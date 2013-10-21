#include "StdAfx.h"
#include "kg3dmaterial.h"
#include "kg3dmodel.h"
#include "kg3dscenetable.h"
#include "kg3dtexturetable.h"
#include "kg3dmaterialtable.h"
#include "kg3denginemanager.h"
#include "KG3DRenderState.h"
#include "kg3dgraphicstool.h"
#include "KG3DGraphiceEngine.h"
#include "kg3dmaterialex.h"
#include "KG3DEffectFileTable.h"
#include "KG3DRenderGlobalState.h"
#include "MathTool.h"
#include "..\KG3DMultiThreadLoader.h"
#include "KG3DFileLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern KG3DSceneTable  g_cSceneTable;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern KG3DMaterial::KG3DMaterialSubset* g_pSubMatLast;
extern BOOL g_bForceDisableCull;//强制双面，绘制阴影的时候用

BOOL g_EnableLoadTexture = TRUE;

float g_fHightLight = 0.25f;

static char* g_strTextureType[] = {
    "*MAP_NORMAL"
};

static KG3DMaterial::enuShaderType g_ShaderTypes[] = {
    KG3DMaterial::MAT_SHADER_TYPE_NORMAL_NOSKIN,
    KG3DMaterial::MAT_SHADER_TYPE_NONE
};

KG3DMaterial::KG3DMaterial(void)
{
    m_dwID = 0;
    m_dwType = 0;
    m_ulRefCount = 0;
    m_pMaterials = NULL;
    m_dwNumMaterials = 0;
    m_pMaterialsMulti = NULL;
    m_dwNumMaterialsMulti = 0;
    m_nLastEffect = -1;
    m_ShaderType = MAT_SHADER_TYPE_NONE;
    m_bSortAsSFX = FALSE;
    m_bHasDetail = FALSE;
    m_eLoadingState = STATE_NONE;
    m_bInMultiThreadLoadList = FALSE;
    m_uOption = 0;
    bTest = FALSE;
	m_bTextureLoaded = FALSE;
    m_nMaxMtlIndex = -1;
    m_pstrTextureName = NULL;
    m_bDetailWholeColorCast = TRUE;
    m_strOrgTextureFilePath = "";
}

KG3DMaterial::~KG3DMaterial(void)
{
    ASSERT(!bTest);
    UnInit();
}

DWORD KG3DMaterial::KG3DMaterialSubset::ComputeHashValue()
{
    if(m_lpTextures[0])
        m_dwMaterialID = m_lpTextures[0]->GetID();
    else
        m_dwMaterialID = 0;
    return m_dwMaterialID;
}

ULONG STDMETHODCALLTYPE KG3DMaterial::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}


HRESULT KG3DMaterial::OnReferenceZero()
{
    ASSERT(m_ulRefCount>=0);
    g_cMaterialTable.ReleaseResource(this, RELEASE_INQUEUE);
    return S_OK;
}

ULONG STDMETHODCALLTYPE KG3DMaterial::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero();
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DMaterial::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//初始化,清除数据
HRESULT KG3DMaterial::Init()
{
    m_scName = "";
    m_pMaterials = NULL;

    return S_OK;
}

HRESULT KG3DMaterial::UnInit()
{
    if (m_pMaterials != NULL)
    {
        for (size_t i = 0; i < m_dwNumMaterials; i++)
        {
            if (g_pSubMatLast == (&(m_pMaterials[i])))
            {
                g_pSubMatLast = NULL;
            }
        }
    }
    SAFE_DELETE_ARRAY(m_pMaterialsMulti);
    m_dwNumMaterialsMulti = 0;
    SAFE_DELETE_ARRAY(m_pMaterials);
    SAFE_DELETE_ARRAY(m_pstrTextureName);
    m_dwNumMaterials = 0;
    m_nLastEffect = -1;
    m_ShaderType = MAT_SHADER_TYPE_NONE;
    m_eLoadingState = STATE_NONE;
    m_uOption = 0;

	for (size_t i=0;i<m_vecDetailDefine.size();i++)
	{
		SAFE_DELETE(m_vecDetailDefine[i]);
	}
	m_vecDetailDefine.clear();

    return S_OK;
}

void KG3DMaterial::_ModifyLoadOption(DWORD &dwTextureLoadOption,
                                    DWORD dwSubsetID)
{
    _MtlRenderTwiceAlphaOption *pRenderTwiceAlpha = NULL;
    KG_PROCESS_ERROR(dwSubsetID < m_dwNumMaterials);

    if (m_ShaderType == MAT_SHADER_TYPE_NORMAL_NOSKIN)
    {
        //带Normalmap的贴图， 要按照原图来加载
        dwTextureLoadOption |= (TEXOP_DISABLEDTX | TEXOP_DISABLESCALE);
    }

    pRenderTwiceAlpha = m_pMaterials[dwSubsetID].GetAlphaRenderTwice();
    KGLOG_PROCESS_ERROR(pRenderTwiceAlpha);

Exit0:
    ;
}

HRESULT KG3DMaterial::_LoadTexture(DWORD dwOption)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwShaderType = MAT_SHADER_TYPE_NONE;
    DWORD dwTextureLoadOption = (dwOption & MLO_TEXTURE_MULTI) ? TEXOP_RELOAD : 0;
    for (DWORD i = 0; i < m_dwNumMaterials; i++)
    {
        KG3DMaterialSubset& SubMat = m_pMaterials[i];
        //GetShaderType
        for (DWORD j = 0; j < SubMat.m_dwNumUsedTexture; j++)
        {
            DWORD k = 0;
            DWORD dwSize = sizeof(g_strTextureType) / sizeof(g_strTextureType[0]);
            for (k = 0; k < dwSize; k++)
            {
                if (!strcmp(g_strTextureType[k], SubMat.m_scTextureType[j].c_str()))
                    break;
            }
            if (k < dwSize &&
                dwShaderType < static_cast<DWORD>(g_ShaderTypes[k]))
                dwShaderType = g_ShaderTypes[k];
        }
    }

	SetShaderType(dwShaderType);

	//////////////////////////////////////////////////////////////////////////
	for (DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		KG3DMaterialSubset& SubMat = m_pMaterials[i];
		for (DWORD j = 0; j < SubMat.m_dwNumUsedTexture; j++)
		{
			_ModifyLoadOption(dwTextureLoadOption, i);

			ASSERT(!SubMat.m_lpTextures[j]);

            if(j==1 && (strcmp(SubMat.m_scTextureType[j].c_str(), "*MAP_AMBIENT") == 0))
            {
                //跳过旧文件中的光照图
            }
            else
            {
			    hrRetCode = g_cTextureTable.LoadResourceFromFile(SubMat.m_scTextureName[j].c_str(),
				    0,
				    dwTextureLoadOption,
				    (IKG3DResourceBase**)&(SubMat.m_lpTextures[j]));
			    //KGLOG_COM_PROCESS_ERROR(hrRetCode); // if failed, the editor can't open the material to correct the file
			    if (FAILED(hrRetCode)) 
			    {
				    KGLogPrintf(KGLOG_ERR, "MTL:%s, Load Texture %s failed", m_scName.c_str(), SubMat.m_scTextureName[j].c_str());
			    }
            }

			if(j==0 )
			{
				if((g_cEngineOption.bEnableSpeedTreeNormalMap && (m_uOption & MLO_MESH_SPEEDTREE)) || g_cEngineOption.bEnableNormalMap)
				{
					char strFile[MAX_PATH];
					char strDrive[MAX_PATH];
					char strPath[MAX_PATH];
					char strExt[MAX_PATH];
					_splitpath_s(SubMat.m_scTextureName[j].c_str(),
						strDrive,
						MAX_PATH,
						strPath,
						MAX_PATH,
						strFile,
						MAX_PATH,
						strExt,
						MAX_PATH);
					char szNormalName[MAX_PATH];
					wsprintf(szNormalName,"%s%s%s_n%s",strDrive,strPath,strFile,strExt);
					SAFE_RELEASE(SubMat.m_lpNormalMap);
					g_cTextureTable.LoadResourceFromFile(szNormalName,0,TEXOP_RELOAD,(IKG3DResourceBase**)&SubMat.m_lpNormalMap );
				}
			}
		}
		SubMat.ComputeHashValue();
	}

   
    hrResult = S_OK;
//Exit0:
    return hrResult;
}

//////////////////////////////////////////////////////////////////////////
//加载,保存
HRESULT KG3DMaterial::LoadFromFile(const char cszFileName[],
                                   unsigned uFileNameHash,
                                   unsigned uOption)
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hResult  = E_FAIL;

    m_scName = cszFileName;
    m_uOption = uOption;

    if((uOption & MLO_TEXTURE_MULTI) && (g_eEngineOption.bUseMultiThreadLoad))
    {
        g_cMultiThreadLoader.TryMultiThreadLoad_Material(this);
    }
    else
    {
        hResult = _LoadFromFileDirect(cszFileName, uOption);
        KG_COM_PROCESS_ERROR(hResult);
    }
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DMaterial::LoadFromFileMultiThread()
{
    HRESULT hResult  = E_FAIL;

	if(m_eLoadingState == STATE_LOADCOMPLETE)
	{
		return S_OK;
	}

    hResult = _LoadFromFileDirect(m_scName.c_str(), m_uOption);
    KG_COM_PROCESS_ERROR(hResult);

    m_eLoadingState = STATE_LOADCOMPLETE;
Exit0:
    return hResult;
}

HRESULT KG3DMaterial::_LoadFromFileDirect(const char cszFileName[], unsigned uOption)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    DWORD dwNowTime = timeGetTime();
    IKG_Buffer *piFileBuf = NULL;
    KG3DBufferReader BufferReader;
    int nHasDetail = false;
    int nIsSortAsSFX = false;
    DWORD dwMask = 0;
    DWORD dwBlockLength = 0;

    ASSERT(cszFileName);

    ASSERT(!m_pMaterials);

    nRetCode = g_FileLoader.LoadImmediately(cszFileName, &piFileBuf);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BufferReader.SetBuffer(piFileBuf);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BufferReader.Read(&dwMask, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(dwMask == 0x4D41544C);//MATL对应的ASECC代码

    //Read block length
    nRetCode = BufferReader.Read(&dwBlockLength, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);

    //read num materials
    nRetCode = BufferReader.Read(&m_dwNumMaterials, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_dwNumMaterials <= 128); // just for check file error, normally this value < 10

    m_pMaterials = new KG3DMaterialSubset[m_dwNumMaterials];
    KGLOG_PROCESS_ERROR(m_pMaterials);

    m_pstrTextureName = new string[m_dwNumMaterials];
    KGLOG_PROCESS_ERROR(m_pstrTextureName);

    for (DWORD i = 0; i < m_dwNumMaterials; i++)
    {
        hrRetCode = m_pMaterials[i].LoadFromBuffer(
            cszFileName, &BufferReader, &nHasDetail, &nIsSortAsSFX
        );
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        m_pstrTextureName[i] = m_pMaterials[i].m_scTextureName[0];

        //m_bHasDetail |= nHasDetail;
        //m_bSortAsSFX |= nIsSortAsSFX;
		m_bHasDetail = nHasDetail || m_bHasDetail;
		m_bSortAsSFX = nIsSortAsSFX || m_bSortAsSFX;
    }
    // maybe still some data in file, old version use it, now not need

	if(g_EnableLoadTexture)
	{
		m_bTextureLoaded = TRUE;
		hrRetCode = _LoadTexture(uOption);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);
	}
	else
	{
		m_bTextureLoaded = FALSE;
	}

    //////////////////////////////////////////////////////////////////////////
    if(m_bHasDetail)
    {
        char szName[MAX_PATH];
        wsprintf(szName,"%s.ini",cszFileName);

        LoadMaterialDetails(szName);
    }

    m_eLoadingState = STATE_LOADCOMPLETE;
    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(piFileBuf);
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_pMaterials);
    }

    if (FAILED(hrResult) && cszFileName)
    {
        KGLogPrintf(KGLOG_ERR, "Load Metrial Failed:%s", cszFileName);
    }

    DWORD dwCost = timeGetTime() - dwNowTime;
    if(dwCost >= 30)
    {
        //KGLogPrintf(KGLOG_ERR, "%d KG3DMaterial::LoadFromFile %s.",dwCost,cszFileName);
    }
    return hrResult;
}

void KG3DMaterial::KG3DMaterialSubset::ClearTextureStage(DWORD dwStageIndex)
{
    KG_PROCESS_ERROR(dwStageIndex < MAX_NUM_TEXTURESTAGE);
    
    for (size_t i = 0; i < m_pTextureOptions[dwStageIndex].size(); ++i)
    {
        KG_DELETE(m_pTextureOptions[dwStageIndex][i]);
    }
    m_pTextureOptions[dwStageIndex].clear();

    SAFE_RELEASE(m_lpTextures[dwStageIndex]);

    for(DWORD i = dwStageIndex + 1; i < MAX_NUM_TEXTURESTAGE; i++)
    {
        m_scTextureName[i - 1] = m_scTextureName[i];
        m_scTextureType[i - 1] = m_scTextureType[i];
        m_lpTextures[i - 1] = m_lpTextures[i];
        m_pTextureOptions[i - 1] = m_pTextureOptions[i];

        m_lpTextures[i] = NULL;
        m_pTextureOptions[i].clear();
        m_scTextureType[i].clear();
        m_scTextureName[i].clear();
    }
Exit0:
    return;
}

static const float s_CurrentTextureFileNameVersion = 2.0f;
HRESULT KG3DMaterial::SaveToFile(const char cszFileName[],   unsigned uOption)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwMask = 0x4D41544C;//MATL对应的ASECC代码
    DWORD dwBlockEnd  = 0xFFFFFFFF;
    DWORD dwBlockLength = 0;
    FILE* fpFile = NULL;

    if(m_vecDetailDefine.size()>0)
        m_bHasDetail = TRUE;
    else
        m_bHasDetail = FALSE;

    fpFile = fopen(cszFileName, "wb");
    KG_PROCESS_ERROR(fpFile);

    fwrite(&dwMask, sizeof(DWORD), 1, fpFile);
    //计算并写入快长度
    dwBlockLength += sizeof(DWORD) * 2;//子部分数目，结束标志

    for(DWORD dwSubIndex = 0; dwSubIndex < m_dwNumMaterials; dwSubIndex++)
    {
        dwBlockLength += sizeof(float) * 17
                    + sizeof(DWORD)//四种颜色，反光强度，帖图数目
                    + 8 * (sizeof(float) + sizeof(char) * MAX_PATH);//帖图信息
    }

    fwrite(&dwBlockLength, sizeof(DWORD), 1, fpFile);
    fwrite(&m_dwNumMaterials, sizeof(DWORD), 1,fpFile);                  //子部分数目
    for(DWORD dwSubIndex=0; dwSubIndex < m_dwNumMaterials; dwSubIndex++)
    {
        hrRetCode = m_pMaterials[dwSubIndex].SaveToFile(fpFile, m_bHasDetail, m_bSortAsSFX);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    fwrite(&dwBlockEnd,sizeof(DWORD),1,fpFile);

    //////////////////////////////////////////////////////////////////////////
    if(m_bHasDetail)
    {
        char szName[MAX_PATH];
        wsprintf(szName,"%s.ini",cszFileName);
        SaveMaterialDetails(szName);
    }
    hrResult = S_OK;

Exit0:
    if(fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return hrResult;
}

//////////////////////////////////////////////////////////////////////////
//ID,type
DWORD KG3DMaterial::GetID()
{
    return m_dwID;
}

unsigned KG3DMaterial::GetType()
{
    return m_dwType;
}

//////////////////////////////////////////////////////////////////////////
//内存管理
HRESULT KG3DMaterial::PrepareRender(unsigned uOption) //压缩数据
{
    return S_OK;
}
HRESULT KG3DMaterial::PostRender()//恢复数据，对象可以立即被使用
{
    return S_OK;
}
HRESULT KG3DMaterial::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
    return S_OK;
}
unsigned   KG3DMaterial::GetLastUseTime()//查询上次使用的时间
{
    return 0;
}

unsigned   KG3DMaterial::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
    return 0;
}

HRESULT KG3DMaterial::KG3DMaterialSubset::RestoreMaterial()
{
    g_GetRenderStateMgr().Restore();
    return S_OK;
}

HRESULT KG3DMaterial::KG3DMaterialSubset::RestoreBillboardCloudMaterial()
{
    g_GetRenderStateMgr().Restore();
    return S_OK;
}

LPDIRECT3DTEXTURE9 KG3DMaterial::GetSpeedTreeLeafTexture(DWORD MaterialID)
{
    KG3DMaterialSubset* pMaterial = NULL;
    //ASSERT(MaterialID >= m_dwNumMaterials);
    pMaterial = &m_pMaterials[MaterialID];

    if (pMaterial->m_lpTextures[0])
    {
        return pMaterial->m_lpTextures[0]->GetTexture();
    }

    return NULL;
}

DWORD KG3DMaterial::KG3DMaterialSubset::GetAlphaType()
{
    void* pOption = NULL;

    pOption = FindOption(OPTION_MATERIAL_ALPHAOPERATION, 0);
    if(!pOption)
    {
        return 1;
    }
    else
    {
        _MtlOption* pOp = (_MtlOption*)pOption;
        _MtlAlphaOperation* pAlpha = (_MtlAlphaOperation*)pOp->pData;

        if(pAlpha->EnableAlphaBlend)
            return 4;
        else if (pAlpha->EnableAlphaTest)
        {
            return 2;
        }
    }
    return 1;
}

KG3DTexture* KG3DMaterial::KG3DMaterialSubset::GetSubMeshTexture()
{
    KG3DTexture* pTexture = m_lpTextures[0];
    return pTexture;
}

KG3DTexture* KG3DMaterial::KG3DMaterialSubset::GetSubMeshTextureNormal()
{
    return m_lpNormalMap;
}


HRESULT KG3DMaterial::KG3DMaterialSubset::SetMaterialEffect(float fAlpha,DWORD dwOption,int nShader,LPD3DXEFFECT pEffect)
{
    return S_OK;
}

HRESULT KG3DMaterial::KG3DMaterialSubset::SetBillboardCloudMaterial()
{
    KG3DRenderState& RenderStateMgr = g_GetRenderStateMgr();
    D3DMATERIAL9 matRuntime;

    if (m_dwOption & MATERIAL_OPTION_HIDE)
        return S_FALSE;

    // cull
    RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // alpha
    RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    RenderStateMgr.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RenderStateMgr.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
    RenderStateMgr.SetRenderState(D3DRS_ALPHAREF, 0x80);
    RenderStateMgr.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    RenderStateMgr.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    RenderStateMgr.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    RenderStateMgr.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);


    // z buffer
    if (m_dwOption & MATERIAL_OPTION_ZBUFFER_FALSE)
        RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    else if (m_dwOption & MATERIAL_OPTION_ZBUFFER_USEW)
        RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
    else
        RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);


    // light
    if (m_dwOption & MATERIAL_OPTION_LIGHTING)
        RenderStateMgr.SetRenderState(D3DRS_LIGHTING, TRUE);
    else
        RenderStateMgr.SetRenderState(D3DRS_LIGHTING, FALSE);

    // fill
    if (m_dwOption & MATERIAL_OPTION_FILL_SOLID)
        RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    else if (m_dwOption & MATERIAL_OPTION_FILL_POINT)
        RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
    else if (m_dwOption & MATERIAL_OPTION_FILL_WIREFRAME)
        RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    // hight light
    if (m_dwOption & MATERIAL_OPTION_SPECULARENABLE)
    {
        RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
        RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    }
    else
    {
        RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);


    // z write
    if (m_dwOption & MATERIAL_OPTION_ZWRITEDISABLE)
        RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    else
        RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    memcpy(&matRuntime, &m_sMaterial9, sizeof(D3DMATERIAL9));

    matRuntime.Ambient.a = matRuntime.Ambient.r = matRuntime.Ambient.g = matRuntime.Ambient.b = 1;
    matRuntime.Diffuse = matRuntime.Ambient;
    matRuntime.Emissive =matRuntime.Ambient;
    g_pd3dDevice->SetMaterial(&matRuntime);

    return S_OK;
}


HRESULT KG3DMaterial::KG3DMaterialSubset::SetMaterial(float fAlpha, DWORD dwOption)
{
    HRESULT hr = E_FAIL;
    _MtlOption* pOpt = NULL;
    DWORD dwTime = g_cGraphicsTool.GetNowTime() * 2;
    KG3DTexture* pTexture = NULL;
    KG3DRenderState& RenderStateMgr = g_GetRenderStateMgr();
    D3DMATERIAL9 matRuntime;


    if (m_dwOption & MATERIAL_OPTION_HIDE)
    {
        return S_FALSE;
    }


	KG_PROCESS_ERROR(TRUE);//避免warning
    //裁减信息
	if(g_bForceDisableCull)
	{
		hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	else
	{
		if (m_dwOption & MATERIAL_OPTION_CULL_NONE)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
		else if (m_dwOption & MATERIAL_OPTION_CULL_CCW)
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else
		{
			hr = RenderStateMgr.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
	}
    //Z buffer信息
    if (m_dwOption & MATERIAL_OPTION_ZBUFFER_FALSE)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    }
    else if (m_dwOption & MATERIAL_OPTION_ZBUFFER_USEW)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
    }
    else
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    }

    //是否打开光照
    if (m_dwOption & MATERIAL_OPTION_LIGHTING)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_LIGHTING, TRUE);
    }
    else
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_LIGHTING, FALSE);
    }

    //填充模式
    if (m_dwOption & MATERIAL_OPTION_FILL_SOLID)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }
    else if (m_dwOption & MATERIAL_OPTION_FILL_POINT)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
    }
    else if (m_dwOption & MATERIAL_OPTION_FILL_WIREFRAME)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }

    //高光高亮模式

    if (m_dwOption & MATERIAL_OPTION_SPECULARENABLE)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
        hr = RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
    }
    else
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        hr = RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }


    //Z write

    if (m_dwOption & MATERIAL_OPTION_ZWRITEDISABLE)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }
    else
    {
      hr = RenderStateMgr.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }

    _MtlAlphaOperation *pAlphaOp = GetAlphaOperation();
    float fTemp = static_cast<float>(pAlphaOp->BackAlphaTestRef) * fAlpha;
    pAlphaOp->AlphaTestRef = static_cast<DWORD>(fTemp);

    int nAddModeFlag = false;

    for(DWORD dwOptIndex = 0; dwOptIndex < m_dwNumOptions; dwOptIndex++)
    {
        pOpt = m_pMatOptions[dwOptIndex];
        hr = SetOption(0, pOpt);

        if (pOpt->Type == OPTION_MATERIAL_ALPHAOPERATION)
        {
            _MtlAlphaOperation* pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOpt->pData);
            if (pMtlAlphaOp->BlendOperator == D3DBLENDOP_ADD &&
                pMtlAlphaOp->SrcBlend == D3DBLEND_SRCALPHA &&
                pMtlAlphaOp->DestBlend == D3DBLEND_ONE)
            nAddModeFlag = true;
        }
    }
    memcpy(&matRuntime, &m_sMaterial9, sizeof(D3DMATERIAL9));

    if (fAlpha < 1.0f && !nAddModeFlag)
    {
        hr = RenderStateMgr.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        hr = RenderStateMgr.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        hr = RenderStateMgr.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    }

    matRuntime.Diffuse.a = fAlpha;
    RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    if (dwOption & RENDER_HIGHTLIGHT)
    {
        matRuntime.Diffuse.a = fAlpha;
        matRuntime.Emissive.a = 1.0f;
        matRuntime.Emissive.r = matRuntime.Emissive.g = matRuntime.Emissive.b = g_fHightLight;
    }

    hr = g_pd3dDevice->SetMaterial(&matRuntime);

    KG3DTexture *pNullTexture = g_cTextureTable.GetNullTexture();
    ASSERT(pNullTexture);

    for(int i = 0; i < 8; i++)
    {
        if (!m_lpTextures[i])
        {
            if (i == 0)
            {
                hr = g_pd3dDevice->SetTexture(i, pNullTexture->GetTexture());
            }
            else
            {
                hr = g_pd3dDevice->SetTexture(i, NULL);
            }
            hr = RenderStateMgr.SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
            hr = RenderStateMgr.SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            break;
        }

        pTexture = m_lpTextures[i];
        hr = pTexture->Update(1, dwTime);
        pTexture->PreLoad();

        hr = g_pd3dDevice->SetTexture(i, pTexture->GetTexture());

        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        hr = RenderStateMgr.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

        for(DWORD dwTOptionIndex = 0; dwTOptionIndex < m_pTextureNumOption[i]; dwTOptionIndex++)
        {
            pOpt = m_pTextureOptions[i][dwTOptionIndex];
            hr = SetOption(i, pOpt);
        }
    }
    hr = S_OK;
Exit0:
    return hr;
}


HRESULT KG3DMaterial::KG3DMaterialSubset::SetOption(DWORD dwTextureStage, _MtlOption* pOption)
{
    HRESULT hr = E_FAIL;
    D3DXMATRIX Mat1;
    D3DXMATRIX Mat2;
    KG3DRenderState& RenderState = g_GetRenderStateMgr();

    KG_PROCESS_ERROR(dwTextureStage < 8 && pOption);

    switch(pOption->Type)
    {
    case OPTION_MATERIAL_ZFUN:
        {
            hr = RenderState.SetRenderState(D3DRS_ZFUNC, *(DWORD*)(pOption->pData));
            break;
        }
    case OPTION_MATERIAL_CULLING:
        {
            ASSERT(0);
            //_MatCull* pMatCull = reinterpret_cast<_MatCull*>(pOption->pData);
            //hr = RenderState.SetRenderState(D3DRS_CULLMODE, pMatCull->CullMode);
            break;
        }
    case OPTION_MATERIAL_ALPHABLEND:
        {
            ASSERT(0);
            //_MatBlend* pMatBlend = reinterpret_cast<_MatBlend*>(pOption->pData);

            //hr = hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMatBlend->SrcBlend);
            //hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMatBlend->DestBlend);
            //hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            break;
        }
    case OPTION_MATERIAL_ALPHAOPERATION:
        {
            _MtlAlphaOperation* pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOption->pData);
            hr = RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, pMtlAlphaOp->EnableAlphaBlend);
            hr = RenderState.SetRenderState(D3DRS_SRCBLEND, pMtlAlphaOp->SrcBlend);
            hr = RenderState.SetRenderState(D3DRS_DESTBLEND, pMtlAlphaOp->DestBlend);
            hr = RenderState.SetRenderState(D3DRS_BLENDOP, pMtlAlphaOp->BlendOperator);
            hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR, pMtlAlphaOp->BlendFactor);
            hr = RenderState.SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, pMtlAlphaOp->EnableSeparateAlphaBlend);
            hr = RenderState.SetRenderState(D3DRS_BLENDOPALPHA, pMtlAlphaOp->BlendOperatorAlpha);
            hr = RenderState.SetRenderState(D3DRS_SRCBLENDALPHA, pMtlAlphaOp->SrcBlendAlpha);
            hr = RenderState.SetRenderState(D3DRS_DESTBLENDALPHA, pMtlAlphaOp->DestBlendAlpha);
            hr = RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, pMtlAlphaOp->EnableAlphaTest);
            hr = RenderState.SetRenderState(D3DRS_ALPHAREF, pMtlAlphaOp->AlphaTestRef);
            hr = RenderState.SetRenderState(D3DRS_ALPHAFUNC, pMtlAlphaOp->AlphaFunction);
            break;
        }
    case OPTION_TEXTURE_OPERATION:
        {
            _TextureOp* pTextureOp = reinterpret_cast<_TextureOp*>(pOption->pData);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLOROP, pTextureOp->ColorOp);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAOP, pTextureOp->AlphaOp);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG1, pTextureOp->ColorArg1);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG2, pTextureOp->ColorArg2);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG1, pTextureOp->AlphaArg1);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG2, pTextureOp->AlphaArg2);
            //hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
            break;
        }
    case OPTION_TEXTURE_OPERATIONEX:
        {
            _MtlTextureOpEx* pTextureOpEx = reinterpret_cast<_MtlTextureOpEx*>(pOption->pData);
            KG_PROCESS_ERROR(pTextureOpEx);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLOROP, pTextureOpEx->ColorOp);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAOP, pTextureOpEx->AlphaOp);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG1, pTextureOpEx->ColorArg1);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG2, pTextureOpEx->ColorArg2);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG1, pTextureOpEx->AlphaArg1);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG2, pTextureOpEx->AlphaArg2);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_ALPHAARG0, pTextureOpEx->AlphaArg0);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_COLORARG0, pTextureOpEx->ColorArg0);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_RESULTARG, pTextureOpEx->Result);
            hr = RenderState.SetTextureStageState(dwTextureStage, D3DTSS_CONSTANT , pTextureOpEx->Constant);
            //hr = RenderState.SetRenderState(D3DRS_BLENDFACTOR,pTextureOp->TFactor);
            break;
        }
    case OPTION_TEXTURE_MAPMETHOD:
        {
            _TextureMap* pMap = reinterpret_cast<_TextureMap*>(pOption->pData);
            hr = RenderState.SetTextureStageState(dwTextureStage,D3DTSS_TEXCOORDINDEX,pMap->TextureMap);
            break;
        }
    case OPTION_TEXTURE_TRANSFROM:
        {
            float fNowTimeInSecond = g_cGraphicsTool.GetNowTime() * 0.001f;
            _TextureTf* pTF = reinterpret_cast<_TextureTf*>(pOption->pData);

            hr = RenderState.SetTextureStageState(dwTextureStage,
                                                    D3DTSS_TEXTURETRANSFORMFLAGS,
                                                    D3DTTFF_COUNT3|D3DTTFF_PROJECTED
                                                    );

            D3DXMatrixScaling(&Mat1, pTF->Scaling.x, pTF->Scaling.y, pTF->Scaling.z);
            D3DXMatrixRotationYawPitchRoll(&Mat2,
                                           pTF->Rotation.x * fNowTimeInSecond,
                                           pTF->Rotation.y * fNowTimeInSecond,
                                           pTF->Rotation.z*fNowTimeInSecond);
            D3DXMatrixMultiply(&Mat1, &Mat1, &Mat2);
            Mat1._41 = pTF->Position.x * fNowTimeInSecond;
            Mat1._42 = pTF->Position.y * fNowTimeInSecond;
            Mat1._43 = pTF->Position.z * fNowTimeInSecond;

            switch(dwTextureStage)
            {
            case 0:
                hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE0,&Mat1);
                break;
            case 1:
                hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE1,&Mat1);
                break;
            case 2:
                hr = g_pd3dDevice->SetTransform(D3DTS_TEXTURE2,&Mat1);
                break;
            case 3:
                g_pd3dDevice->SetTransform(D3DTS_TEXTURE3,&Mat1);
                break;
            }
            break;
        }
    }
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DMaterial::KG3DMaterialSubset::_LoadMaterialOption(KG3DBufferReader *pBufferReader)
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    DWORD dwType = 0;

    KG_ASSERT_EXIT(pBufferReader);

    nRetCode = pBufferReader->Read(&dwType, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);

    switch(dwType)
    {
    case OPTION_MATERIAL_ZFUN:
        {
            _MtlOption* pOption = static_cast<_MtlOption*>(GetMtlOption(dwType, 0xffffffff));
            KG_PROCESS_ERROR(pOption);
            nRetCode = pBufferReader->Read(pOption->pData, sizeof(DWORD));
            KG_PROCESS_ERROR(nRetCode);
            break;
        }
    case OPTION_MATERIAL_CULLING:
        {
            _MatCull *pCullOption = NULL;

            nRetCode = pBufferReader->ReferenceRead((void **)&pCullOption, sizeof(*pCullOption));
            KG_PROCESS_ERROR(nRetCode);

            //pOption->pData = pMatCull;
            //move data into material option
            m_dwOption &= ~MATERIAL_OPTION_CULL_CCW;
            m_dwOption &= ~MATERIAL_OPTION_CULL_CW;
            m_dwOption &= ~MATERIAL_OPTION_CULL_NONE;
            switch(pCullOption->CullMode)
            {
            case D3DCULL_CCW:
                {
                    m_dwOption |= MATERIAL_OPTION_CULL_CCW;
                    break;
                }
            case D3DCULL_CW:
                {
                    m_dwOption |= MATERIAL_OPTION_CULL_CW;
                    break;
                }
            case D3DCULL_NONE:
                {
                    m_dwOption |= MATERIAL_OPTION_CULL_NONE;
                    break;
                }
            }
            break;
        }
    case OPTION_MATERIAL_ALPHABLEND:
        {
            _MatBlend *pBlendOption = NULL;
            _MtlAlphaOperation* pMtlAlphaOp = GetAlphaOperation();
            KGLOG_PROCESS_ERROR(pMtlAlphaOp);

            nRetCode = pBufferReader->Read((void **)&pBlendOption, sizeof(*pBlendOption));
            KG_PROCESS_ERROR(nRetCode);

            pMtlAlphaOp->SrcBlend  = pBlendOption->SrcBlend;
            pMtlAlphaOp->DestBlend = pBlendOption->DestBlend;

            pMtlAlphaOp->EnableAlphaBlend = TRUE;
            break;
        }
    case OPTION_MATERIAL_ALPHAOPERATION:
        {
            _MtlAlphaOperation* pAlphaOption = GetAlphaOperation();
            KG_PROCESS_ERROR(pAlphaOption);

            nRetCode = pBufferReader->Read(pAlphaOption, sizeof(_MtlAlphaOperation));
            KG_PROCESS_ERROR(nRetCode);
            pAlphaOption->BackAlphaTestRef = pAlphaOption->AlphaTestRef;
            break;
        }
    case OPTION_MATERIAL_ALPHARENDERTWICE:
        {
            _MtlRenderTwiceAlphaOption *pAlphaOption = GetAlphaRenderTwice();
            KG_PROCESS_ERROR(pAlphaOption);

            nRetCode = pBufferReader->Read(pAlphaOption, sizeof(_MtlRenderTwiceAlphaOption));
            KG_PROCESS_ERROR(nRetCode);
            break;
        }
    case OPTION_MATERIAL_MSEFFECT:
        {
            struct _SAVED_EFFECT
            {
                char szFileName[MAX_PATH];
                int  nTechnique;
                int  nEffParaIndex;
            };
            nRetCode = pBufferReader->Seek(sizeof(_SAVED_EFFECT), SEEK_CUR);
            KGLOG_PROCESS_ERROR(nRetCode);
            break;
        }
    }

    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DMaterial::KG3DMaterialSubset::_LoadTexturOption(
    KG3DBufferReader *pBufferReader, DWORD dwTextureStageIndex
)
{
    HRESULT hr = E_FAIL;
    int nRetCode = 0;
    void* pDataReadIn = NULL;
    _MtlOption* pNewOption = NULL;

    KG_ASSERT_EXIT(pBufferReader);
    KG_PROCESS_ERROR(dwTextureStageIndex < 8);

    pNewOption = new _MtlOption;
    KG_PROCESS_ERROR(pNewOption);

    nRetCode = pBufferReader->Read(&(pNewOption->Type), sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);

    ASSERT(pNewOption->Type != OPTION_MATERIAL_MSEFFECT);

    switch(pNewOption->Type)
    {
    case OPTION_TEXTURE_OPERATION:
        {
            pDataReadIn = new _MtlTextureOp;
            KGLOG_PROCESS_ERROR(pDataReadIn);

            nRetCode = pBufferReader->Read(pDataReadIn, sizeof(_MtlTextureOp));
            KGLOG_PROCESS_ERROR(nRetCode);

            pNewOption->pData = pDataReadIn;
            break;
        }
    case OPTION_TEXTURE_OPERATIONEX:
        {
            pDataReadIn = new _TextureOpEx;
            KG_PROCESS_ERROR(pDataReadIn);

            nRetCode = pBufferReader->Read(pDataReadIn, sizeof(_TextureOpEx));
            KGLOG_PROCESS_ERROR(nRetCode);

            pNewOption->pData = pDataReadIn;
            break;
        }
    case OPTION_TEXTURE_MAPMETHOD:
        {
            pDataReadIn = new _TextureMap;
            KGLOG_PROCESS_ERROR(pDataReadIn);

            nRetCode = pBufferReader->Read(pDataReadIn, sizeof(_TextureMap));
            KGLOG_PROCESS_ERROR(nRetCode);

            pNewOption->pData = pDataReadIn;
            break;
        }
    case OPTION_TEXTURE_TRANSFROM:
        {
            pDataReadIn = new _TextureTf;
            KGLOG_PROCESS_ERROR(pDataReadIn);

            nRetCode = pBufferReader->Read(pDataReadIn, sizeof(_TextureTf));
            KGLOG_PROCESS_ERROR(nRetCode);

            pNewOption->pData = pDataReadIn;
            break;
        }
    }

    m_pTextureOptions[dwTextureStageIndex].push_back(pNewOption);
    hr = S_OK;
Exit0:
    if (FAILED(hr))
    {
        SAFE_DELETE(pDataReadIn);
        SAFE_DELETE(pNewOption);
    }
    return hr;
}

KG3DMaterial::_TextureOpEx::_TextureOpEx()
{
    ColorOp = D3DTOP_SELECTARG1;
    AlphaOp = D3DTOP_DISABLE;
    AlphaArg1 = D3DTA_TEXTURE;
    AlphaArg2 = D3DTA_CURRENT;
    ColorArg1 = D3DTA_TEXTURE;
    ColorArg2 = D3DTA_CURRENT;

    ColorArg0 = D3DTA_CURRENT;
    AlphaArg0 = D3DTA_CURRENT;
    Result = D3DTA_CURRENT;
    Constant = D3DTA_CONSTANT;
}

KG3DMaterial::_TextureOp::_TextureOp()
{
    ColorOp = D3DTOP_SELECTARG1;
    AlphaOp = D3DTOP_DISABLE;
    AlphaArg1 = D3DTA_TEXTURE;
    AlphaArg2 = D3DTA_CURRENT;
    ColorArg1 = D3DTA_TEXTURE;
    ColorArg2 = D3DTA_CURRENT;
    TFactor = 0;
}

KG3DMaterial::_TextureMap::_TextureMap()
{
    TextureMap = D3DTSS_TCI_PASSTHRU;
}

KG3DMaterial::KG3DMaterialSubset::KG3DMaterialSubset()
{

    memset(m_pTextureNumOption, 0, sizeof(DWORD) * cNumID);
    memset(m_lpTextures, 0, sizeof(KG3DTexture*) * cNumID);

    m_dwNumOptions = 0;
    m_dwOption = 0;
    m_dwNumFaces = 0;

    //m_ColorCast.a = 0.f;
    //m_ColorCast.r = 0.f;
    //m_ColorCast.g = 0.f;
    //m_ColorCast.b = 1.f;
    for (int i = 0;i < cNumColorCast;i++)
        m_ColorCast[i] = g_cEngineOption.ColorCast[i];

    m_fSpecPower  = 1.f;
    m_fEmssPower  = 1.f;

    m_dwNumUsedTexture = 0;
    m_dwMaterialID = 0;
    m_dwPassID = 0;
    m_lpNormalMap = NULL;
    m_lpAlphaOperation = NULL;
}

KG3DMaterial::KG3DMaterialSubset::~KG3DMaterialSubset()
{
    for(int i = 0; i < 8; i++)
    {
        SAFE_RELEASE(m_lpTextures[i]);
    }

    SAFE_RELEASE(m_lpNormalMap);

    CleanTextureStageOptions();

    CleanMtlOptions();
    m_lpAlphaOperation = NULL;
};
HRESULT KG3DMaterial::KG3DMaterialSubset::LoadFromBuffer(
    const char cszFileName[], KG3DBufferReader *pBufferReader,
    int *pnRetHasDetail, int *pnRetIsSortAsSFX
)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;
    DWORD dwTextureStageIndex = 0;
    DWORD dwMaterialOptionCount = 0;

    KG_ASSERT_EXIT(cszFileName);
    KG_ASSERT_EXIT(pBufferReader);
    KG_ASSERT_EXIT(pnRetHasDetail);
    KG_ASSERT_EXIT(pnRetIsSortAsSFX);

    // the order in file not same as order in D3DMATERIAL9 :(, must read one by one
    nRetCode = pBufferReader->Read(&m_sMaterial9.Ambient, sizeof(D3DCOLORVALUE));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pBufferReader->Read(&m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pBufferReader->Read(&m_sMaterial9.Specular, sizeof(D3DCOLORVALUE));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pBufferReader->Read(&m_sMaterial9.Emissive, sizeof(D3DCOLORVALUE));
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = pBufferReader->Read(&m_sMaterial9.Power, sizeof(float));
    KGLOG_PROCESS_ERROR(nRetCode);

    NORMALIZE_MAT_POWER(m_sMaterial9.Power);

    nRetCode = pBufferReader->Read(&m_dwOption, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);
    m_dwOption |= MATERIAL_OPTION_FILL_SOLID;

    nRetCode = pBufferReader->Read(&dwMaterialOptionCount, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);
    for (DWORD j = 0; j < dwMaterialOptionCount; j++)
    {
        hrRetCode = _LoadMaterialOption(pBufferReader);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    _MtlRenderTwiceAlphaOption * pOption = GetAlphaRenderTwice();
    if (pOption->bEnable)
    {
        //特殊处理， 为了保证RenderTwiceAlpha的物体在半透明状态下的正常显示
        _MtlAlphaOperation *pAlphaOption = GetAlphaOperation();
        pAlphaOption->AlphaTestRef = 128;
        pAlphaOption->BackAlphaTestRef = 128;
        pAlphaOption->EnableAlphaTest = TRUE;
    }

    nRetCode = pBufferReader->Read(&m_dwNumUsedTexture, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(nRetCode);

    ZeroMemory(m_lpTextures,sizeof(KG3DTexture*)*8);

    for (DWORD dwTextIndex = 0; dwTextIndex < m_dwNumUsedTexture; dwTextIndex++)
    {
        TCHAR strDriver[MAX_PATH];
		TCHAR strTest[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strRetName[MAX_PATH];
        TCHAR *pModify = NULL;
        struct _TEXTURE
        {
            float fAmount;
            char  szTextureType[MAT_TEXTURETYPE_LENGTH];
            char  szTextureFileName[MAT_TEXTUREFILENAME_LENGTH];
            DWORD dwTextureOptionCount;
        } *pTextureInfo = NULL;

        nRetCode = pBufferReader->ReferenceRead((void **)&pTextureInfo, sizeof(*pTextureInfo));
        KGLOG_PROCESS_ERROR(nRetCode);

        if (pTextureInfo->fAmount == 1.0f || pTextureInfo->fAmount == 0.0f)
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

            sprintf_s(strTest, 
				MAX_PATH, 
				"%s%s", 
				strDriver, strPath);
			
			g_pFileFinder->GetAbsPathFromRelativePath(strTest,
                pTextureInfo->szTextureFileName,
                strRetName);

            TCHAR strlowerDef[MAX_PATH];
            strcpy_s(strlowerDef, MAX_PATH, g_Def_WorkDirectory);
            _strlwr_s(strlowerDef, MAX_PATH);
            _strlwr_s(strRetName);
            size_t uSize = 0;
            if (strstr(strRetName, strlowerDef))
            {
                uSize = strlen(strlowerDef);
            }
            pModify = strRetName + uSize;
        }
        else
        {
            pModify = pTextureInfo->szTextureFileName;
        }

        m_pTextureNumOption[dwTextureStageIndex] = pTextureInfo->dwTextureOptionCount;

        for(DWORD j = 0; j < m_pTextureNumOption[dwTextureStageIndex]; j++)
        {
            hrRetCode = _LoadTexturOption(pBufferReader, dwTextureStageIndex);
        }

        if (!strcmp(pTextureInfo->szTextureType, "*MAP_OPACITY"))
        {
            ClearTextureStage(dwTextureStageIndex);
            continue;
        }
        m_scTextureType[dwTextIndex] = pTextureInfo->szTextureType;
        m_scTextureName[dwTextIndex] = pModify;

        m_lpTextures[dwTextIndex] = NULL;

        dwTextureStageIndex++;
    }

    if (!dwTextureStageIndex)
    {
        CleanMtlOptions();
    }


    m_dwNumUsedTexture = dwTextureStageIndex;

    if (m_dwOption & MATERIAL_OPTION_VERSION_2)
    {
        if (m_dwOption & MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST)
        {
            nRetCode = pBufferReader->Read(&m_ColorCast, sizeof(D3DCOLORVALUE)*(KG3DMaterialSubset::cNumColorCast));
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            nRetCode = pBufferReader->Read(&m_ColorCast[0], sizeof(D3DCOLORVALUE));
            KGLOG_PROCESS_ERROR(nRetCode);
            m_dwOption |= MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST;
        }
        nRetCode = pBufferReader->Read(&m_fSpecPower, sizeof(float));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pBufferReader->Read(&m_fEmssPower, sizeof(float));
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *pnRetIsSortAsSFX = !(!(m_dwOption & MATERIAL_OPTION_SORTASSFX));
    *pnRetHasDetail   = !(!(m_dwOption & MATERIAL_OPTION_DETAIL));
    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMaterial::KG3DMaterialSubset::SaveToFile(FILE *fpFile, int nHasDetail, int nIsSortAsSFX)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    char strPathBuffer[256];
    char strType[256];
    char strName[256];
    _MtlOption* pOption = NULL;
    DWORD dwNumTexture = 0;

    KG_ASSERT_EXIT(fpFile);

    fwrite(&m_sMaterial9.Ambient, sizeof(D3DCOLORVALUE), 1, fpFile);
    fwrite(&m_sMaterial9.Diffuse, sizeof(D3DCOLORVALUE), 1, fpFile);
    fwrite(&m_sMaterial9.Specular, sizeof(D3DCOLORVALUE), 1,fpFile);
    fwrite(&m_sMaterial9.Emissive, sizeof(D3DCOLORVALUE), 1, fpFile);
    fwrite(&m_sMaterial9.Power, sizeof(float), 1, fpFile);

    m_dwOption |= MATERIAL_OPTION_VERSION_2;
    m_dwOption |= MATERIAL_OPTION_INCLUDEALLDEFAULTCOLORCAST;
    if(nIsSortAsSFX)
        m_dwOption |= MATERIAL_OPTION_SORTASSFX;
	else
		m_dwOption &= ~(MATERIAL_OPTION_SORTASSFX);

    if(nHasDetail)
        m_dwOption |= MATERIAL_OPTION_DETAIL;
	else
		m_dwOption &= ~(MATERIAL_OPTION_DETAIL);

    fwrite(&m_dwOption,sizeof(DWORD),1,fpFile);//材质选项
    fwrite(&m_dwNumOptions, sizeof(DWORD), 1, fpFile);//选项数目
    for(DWORD dwMOpIndex = 0; dwMOpIndex < m_dwNumOptions; dwMOpIndex++)
    {
        pOption = m_pMatOptions[dwMOpIndex];
        hrRetCode = _SaveOption(fpFile, pOption);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    dwNumTexture = 0;
    for(DWORD dwTexIndex = 0;dwTexIndex < 8; dwTexIndex++)
    {
        if (m_scTextureName[dwTexIndex].empty())
            break;
        dwNumTexture++;
    }

    fwrite(&dwNumTexture, sizeof(DWORD), 1, fpFile);//帖图数目
    for(DWORD dwTextIndex = 0; dwTextIndex < dwNumTexture; dwTextIndex++)
    {
        fwrite(&s_CurrentTextureFileNameVersion, sizeof(float), 1, fpFile);
        wsprintf(strType, "%s", m_scTextureType[dwTextIndex].c_str());
        wsprintf(strName, "%s", m_scTextureName[dwTextIndex].c_str());
        //wsprintf(strPathBuffer, "%s", m_scTextureName[dwTextIndex].c_str());
        //hr = g_pFileFinder->GetRelativePath(cszFileName, (m_scTextureName[dwTextIndex]).c_str(), strFileName);
        g_pFileFinder->GetRelativePath(m_scTextureName[dwTextIndex].c_str(), strPathBuffer, 256);
        fwrite(strType, sizeof(char), 20, fpFile);
        //fwrite(strFileName, sizeof(char), 256, fpFile);
        fwrite(strPathBuffer, sizeof(char), 256, fpFile);

        m_pTextureNumOption[dwTextIndex] =
            static_cast<DWORD>(m_pTextureOptions[dwTextIndex].size());
        fwrite(&m_pTextureNumOption[dwTextIndex], sizeof(DWORD), 1, fpFile);//帖图选项数目
        for(DWORD i_TOption=0;i_TOption<m_pTextureNumOption[dwTextIndex];i_TOption++)
        {
            pOption = m_pTextureOptions[dwTextIndex][i_TOption];
            hrRetCode = _SaveOption(fpFile, pOption);
            KGLOG_COM_PROCESS_ERROR(hrRetCode);
        }
    }

    fwrite(&(m_ColorCast),  sizeof(D3DCOLORVALUE) * (KG3DMaterialSubset::cNumColorCast), 1, fpFile);
    fwrite(&(m_fSpecPower), sizeof(float), 1, fpFile);
    fwrite(&(m_fEmssPower), sizeof(float), 1, fpFile);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
HRESULT KG3DMaterial::KG3DMaterialSubset::_SaveOption(FILE *fpFile, _MtlOption* pOption)
{
    HRESULT hr = E_FAIL;

    ASSERT(fpFile);
    ASSERT(pOption);

    fwrite(&pOption->Type, sizeof(DWORD), 1, fpFile);
    switch(pOption->Type)
    {
    case OPTION_MATERIAL_CULLING:
        {
            _MatCull *pMatCull = reinterpret_cast<_MatCull*>(pOption->pData);
            fwrite(pMatCull, sizeof(_MatCull), 1, fpFile);
            break;
        }
    case OPTION_MATERIAL_ALPHABLEND:
        {
            _MatBlend *pMatBlend = reinterpret_cast<_MatBlend*>(pOption->pData);
            fwrite(pMatBlend, sizeof(_MatBlend), 1, fpFile);
            break;
        }
    case OPTION_MATERIAL_ALPHAOPERATION:
        {
            _MtlAlphaOperation *pMtlAlphaOp = reinterpret_cast<_MtlAlphaOperation*>(pOption->pData);
            fwrite(pMtlAlphaOp, sizeof(_MtlAlphaOperation), 1, fpFile);
            break;
        }
    case OPTION_MATERIAL_ALPHARENDERTWICE:
        {
            _MtlRenderTwiceAlphaOption *pMtlRenderTwiceOp = reinterpret_cast<_MtlRenderTwiceAlphaOption*>(pOption->pData);
            fwrite(pMtlRenderTwiceOp, sizeof(_MtlRenderTwiceAlphaOption), 1, fpFile);
            break;
        }
    case OPTION_MATERIAL_MSEFFECT:
        {
            ASSERT(false);
            break;
        }

    case OPTION_TEXTURE_OPERATION:
        {
            _TextureOp *pTextureOp = (_TextureOp*) pOption->pData;
            fwrite(pTextureOp,sizeof(_TextureOp),1,fpFile);
            break;
        }
    case OPTION_TEXTURE_OPERATIONEX:
        {
            _TextureOpEx *pTextureOpEx = (_TextureOpEx*) pOption->pData;
            fwrite(pTextureOpEx,sizeof(_TextureOpEx),1,fpFile);
            break;
        }
    case OPTION_TEXTURE_MAPMETHOD:
        {
            _TextureMap *pMap = (_TextureMap*) pOption->pData;
            fwrite(pMap,sizeof(_TextureMap),1,fpFile);
            break;
        }
    case OPTION_TEXTURE_TRANSFROM:
        {
            _TextureTf *pTf = (_TextureTf*) pOption->pData;
            fwrite(pTf,sizeof(_TextureTf),1,fpFile);
            break;
        }
    case OPTION_MATERIAL_ZFUN:
        {
            fwrite(pOption->pData, sizeof(DWORD), 1, fpFile);
            break;
        }
    }
    hr = S_OK;
    return hr;
}

void KG3DMaterial::KG3DMaterialSubset::CleanTextureStageOptions(DWORD dwStage)
{
    _MtlOption* pOption = NULL;

    if(dwStage>=8)
        return;

    for(int j = 0; j < static_cast<int>(m_pTextureOptions[dwStage].size()); j++)
    {
        pOption = m_pTextureOptions[dwStage][j];
        SAFE_DELETE(pOption);
    }

    SAFE_RELEASE(m_lpTextures[dwStage]);

    m_pTextureNumOption[dwStage] = 0;
    m_pTextureOptions[dwStage].clear();
}

void KG3DMaterial::KG3DMaterialSubset::CleanTextureStageOptions()
{
    _MtlOption* pOption = NULL;

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < static_cast<int>(m_pTextureOptions[i].size()); j++)
        {
            pOption = m_pTextureOptions[i][j];
            SAFE_DELETE(pOption);
        }
        m_pTextureNumOption[i] = 0;
        m_pTextureOptions[i].clear();
    }
}
void KG3DMaterial::KG3DMaterialSubset::CleanMtlOptions()
{
    _MtlOption* pOption = NULL;

    for(int i = 0; i < static_cast<int>(m_pMatOptions.size()); i++)
    {
        pOption = m_pMatOptions[i];
        SAFE_DELETE(pOption);
    }
    m_dwNumOptions = 0;
    m_pMatOptions.clear();
	m_lpAlphaOperation = NULL;
}


_MtlAlphaOperation* KG3DMaterial::KG3DMaterialSubset::GetAlphaOperation()
{
    if(m_lpAlphaOperation)
        return m_lpAlphaOperation;

    _MtlAlphaOperation* pAlpha = NULL;
    _MtlOption* pOp = NULL;
    for(DWORD i = 0; i < m_dwNumOptions; i++)
    {
        pOp = m_pMatOptions[i];
        if (pOp->Type == OPTION_MATERIAL_ALPHAOPERATION)
        {
            pAlpha = (_MtlAlphaOperation*) pOp->pData;
            goto Exit0;
        }
    }

    pOp = new _MtlOption;
    ASSERT(pOp);
    pAlpha = new _MtlAlphaOperation;
    ASSERT(pAlpha);

    pAlpha->EnableAlphaBlend = FALSE;
    pAlpha->SrcBlend = D3DBLEND_SRCALPHA;
    pAlpha->DestBlend = D3DBLEND_INVSRCALPHA;
    pAlpha->BlendFactor = 0xffffffff;
    pAlpha->BlendOperator = D3DBLENDOP_ADD;

    pAlpha->EnableSeparateAlphaBlend = FALSE;
    pAlpha->BlendOperatorAlpha = D3DBLENDOP_ADD;
    pAlpha->SrcBlendAlpha = D3DBLEND_ONE;
    pAlpha->DestBlendAlpha = D3DBLEND_ZERO;

    pAlpha->EnableAlphaTest = TRUE;
    pAlpha->AlphaTestRef = 0x80;
    pAlpha->BackAlphaTestRef = pAlpha->AlphaTestRef;
    pAlpha->AlphaFunction = D3DCMP_GREATEREQUAL;

    pOp->pData = pAlpha;
    pOp->Type = OPTION_MATERIAL_ALPHAOPERATION;
    m_pMatOptions.push_back(pOp);
    m_dwNumOptions++;
    m_lpAlphaOperation = pAlpha;
Exit0:
    return pAlpha;
}

_MtlRenderTwiceAlphaOption* KG3DMaterial::KG3DMaterialSubset::GetAlphaRenderTwice()
{
    _MtlRenderTwiceAlphaOption *pAlpha = NULL;
    _MtlOption* pOption = NULL;
    for (DWORD i = 0; i < m_dwNumOptions; i++)
    {
        pOption = m_pMatOptions[i];
        if (pOption->Type == OPTION_MATERIAL_ALPHARENDERTWICE)
        {
            pAlpha = (_MtlRenderTwiceAlphaOption*) pOption->pData;
            goto Exit0;
        }
    }
    pAlpha = new _MtlRenderTwiceAlphaOption;
    KG_PROCESS_ERROR(pAlpha);

    pOption = new _MtlOption;
    KG_PROCESS_ERROR(pOption);

    pAlpha->dwAlphaRefPass0 = _MtlRenderTwiceAlphaOption::s_dwRef0;
    pAlpha->dwAlphaRefPass1 = _MtlRenderTwiceAlphaOption::s_dwRef1;
    pAlpha->bEnable = FALSE;

    pOption->Type = OPTION_MATERIAL_ALPHARENDERTWICE;
    pOption->pData = pAlpha;

    m_pMatOptions.push_back(pOption);
    m_dwNumOptions++;
Exit0:
    if (!pOption)
    {
        SAFE_DELETE(pAlpha);
    }
    return pAlpha;
}

KG3DMaterial::_MtlTextureOpEx* KG3DMaterial::KG3DMaterialSubset::GetTextureStageOperation(DWORD dwTextIndex)
{
    ASSERT(dwTextIndex < 8);
    DWORD NumOption = static_cast<DWORD>(m_pTextureOptions[dwTextIndex].size());
    _MtlOption* pOp = NULL;
    _MtlTextureOpEx* pTextureOp = NULL;
    for(DWORD i = 0; i < NumOption; i++)
    {
        pOp = m_pTextureOptions[dwTextIndex][i];
        if (pOp->Type == OPTION_TEXTURE_OPERATIONEX)
        {
            pTextureOp = reinterpret_cast<_MtlTextureOpEx*>(pOp->pData);
            goto Exit0;
        }
    }

    pOp = new _MtlOption;
    ASSERT(pOp);

    pTextureOp = new _MtlTextureOpEx;
    ASSERT(pTextureOp);

    pOp->pData = pTextureOp;
    pOp->Type = OPTION_TEXTURE_OPERATIONEX;
    m_pTextureOptions[dwTextIndex].push_back(pOp);
    m_pTextureNumOption[dwTextIndex]++;
Exit0:
    return pTextureOp;
}


KG3DMaterial::_MatBlend::_MatBlend()
{
    SrcBlend  = D3DBLEND_SRCALPHA;
    DestBlend = D3DBLEND_INVSRCALPHA;
}

KG3DMaterial::_MtlOption::_MtlOption()
{
    Type  =  0;
    pData = NULL;
}

KG3DMaterial::_MtlOption::~_MtlOption()
{
    Type  =  0;
    SAFE_DELETE(pData);
}

KG3DMaterial::_MtlTextureData::_MtlTextureData()
{
    TextureID = 0;
}

KG3DMaterial::_MtlTextureData::~_MtlTextureData()
{
    FileName.clear();
    TextureID = 0;
}

KG3DMaterial::_MtlTextureOpEx::_MtlTextureOpEx()
{
    ColorOp = D3DTOP_SELECTARG1;
    AlphaOp = D3DTOP_DISABLE;
    AlphaArg1 = D3DTA_TEXTURE;
    AlphaArg2 = D3DTA_CURRENT;
    ColorArg1 = D3DTA_TEXTURE;
    ColorArg2 = D3DTA_CURRENT;

    ColorArg0 = D3DTA_CURRENT;
    AlphaArg0 = D3DTA_CURRENT;
    Result = D3DTA_CURRENT;
    Constant = D3DTA_CONSTANT;
}

KG3DMaterial::_MtlTextureOp::_MtlTextureOp()
{
    ColorOp = D3DTOP_SELECTARG1;
    AlphaOp = D3DTOP_DISABLE;
    AlphaArg1 = D3DTA_TEXTURE;
    AlphaArg2 = D3DTA_CURRENT;
    ColorArg1 = D3DTA_TEXTURE;
    ColorArg2 = D3DTA_CURRENT;
    TFactor = 0;
}

KG3DMaterial::_MtlTextureMap::_MtlTextureMap()
{
    TextureMap = D3DTSS_TCI_PASSTHRU;
}

KG3DMaterial::_MtlBlend::_MtlBlend()
{
    SrcBlend  = D3DBLEND_SRCALPHA;
    DestBlend = D3DBLEND_INVSRCALPHA;
}

KG3DMaterial::KG3DMaterialSubset* KG3DMaterial::GetSubMaterial(DWORD dwIndex)
{
    KG3DMaterialSubset* pSubMtl = NULL;;

    if(dwIndex < m_dwNumMaterials)
    {
        pSubMtl = &m_pMaterials[dwIndex];
    }

    return pSubMtl;
}

void NormalizeDir(char* strDir)
{
    KG_PROCESS_ERROR(strDir);

    char* strTemp = strDir;

    strTemp = strstr(strDir, "\\\\");
    if (strTemp == strDir)
    {
        strTemp = strstr(strDir + 2, "\\\\");
    }

    while(strTemp)
    {
        g_pFileFinder->strShiftLeft(strTemp, 0, 1);
        strTemp = strstr(strDir, "\\\\");
    }

    strTemp = strstr(strDir, "\\..");
    while(strTemp)
    {
        char* strTemp1 = max(strTemp - 1, strDir);
        while(strTemp1 > strDir && *strTemp1 != '\\')
        {
            strTemp1--;
        }

        g_pFileFinder->strShiftLeft(strTemp1, 0, strTemp - strTemp1 + 3);
        strTemp = strstr(strDir, "\\..");
    }
Exit0:
    return;
}

void* KG3DMaterial::KG3DMaterialSubset::GetMtlOption(DWORD dwType, DWORD dwExtInfo)
{
    void* pOption = NULL;
    _MtlOption* pNewOption = NULL;
    HRESULT hr = E_FAIL;
    if (dwExtInfo != 0xffffffff)
    {
        KG_PROCESS_ERROR(dwExtInfo < 8);
    }

    pOption = FindOption(dwType, dwExtInfo);
    if (!pOption)
    {
        pNewOption = new _MtlOption;
        KGLOG_PROCESS_ERROR(pNewOption);

        pNewOption->Type = dwType;
        pOption = pNewOption;

        switch(dwType)
        {
        case OPTION_MATERIAL_ZFUN:
            {
                pNewOption->pData = new DWORD;
                KG_PROCESS_ERROR(pNewOption->pData);
                *(DWORD*)(pNewOption->pData) = D3DCMP_LESSEQUAL;
                break;
            }
        }

        if (dwExtInfo == 0xffffffff)
        {
            m_pMatOptions.push_back(pNewOption);
            m_dwNumOptions++;
        }
        else
        {
            KG_PROCESS_ERROR(m_lpTextures[dwExtInfo]);
            m_pTextureOptions[dwExtInfo].push_back(pNewOption);
            m_pTextureNumOption[dwExtInfo]++;
        }

    }

    hr = S_OK;

Exit0:
    if (FAILED(hr))
    {
        SAFE_DELETE(pNewOption);
    }
    return pOption;
}

void* KG3DMaterial::KG3DMaterialSubset::FindOption(DWORD dwType, DWORD dwExtInfo)
{
    void* pReturn = NULL;
    if (dwExtInfo == 0xffffffff)
    {
        //material info
        for(size_t i = 0; i < m_pMatOptions.size(); i++)
        {
            if (m_pMatOptions[i]->Type == dwType)
            {
                pReturn = m_pMatOptions[i];
                break;
            }
        }
    }
    else
    {
        KG_PROCESS_ERROR(dwExtInfo < 8);
        KG_PROCESS_ERROR(m_lpTextures[dwExtInfo]);

        for (size_t i  = 0; i < m_pTextureOptions[dwExtInfo].size(); i++)
        {
            if (m_pTextureOptions[dwExtInfo][i]->Type == dwType)
            {
                pReturn = m_pTextureOptions[dwExtInfo][i];
                break;
            }
        }

    }

Exit0:
    return pReturn;
}

void KG3DMaterial::CopyOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest)
{
    _MtlOption* pSrc = NULL;
    _MtlOption* pDest = NULL;

    KG_PROCESS_ERROR(nSrc < m_dwNumMaterials);
    KG_PROCESS_ERROR(nDest < m_dwNumMaterials);
    KG_PROCESS_ERROR(nSrc != nDest);

    pSrc = static_cast<_MtlOption*>(m_pMaterials[nSrc].GetMtlOption(dwType, dwExtInfo));
    KG_PROCESS_ERROR(pSrc);

    pDest = static_cast<_MtlOption*>(m_pMaterials[nDest].GetMtlOption(dwType, dwExtInfo));
    KG_PROCESS_ERROR(pDest);

    pDest->Type = pSrc->Type;

    switch(pDest->Type)
    {
    case OPTION_MATERIAL_ZFUN:
        {
            memcpy(pDest->pData, pSrc->pData, sizeof(DWORD));
            break;
        }
    }
Exit0:
    return;
}

void KG3DMaterial::SetShaderType(DWORD dwType)
{
    int nVSVersion = 0;
    int nPSVersion = 0;

    m_ShaderType = dwType;

    for (DWORD i = 0; i < m_dwNumMaterials; i++)
    {
        switch (m_ShaderType)
        {
        case MAT_SHADER_TYPE_NONE:
            break;
        case MAT_SHADER_TYPE_SKIN:
            {
                KG3DMaterialSubset &SubMat = m_pMaterials[i];

                nVSVersion = D3DSHADER_VERSION_MAJOR(g_cGraphicsEngine.GetD3DCaps().VertexShaderVersion);
                nPSVersion = D3DSHADER_VERSION_MAJOR(g_cGraphicsEngine.GetD3DCaps().PixelShaderVersion);

                if (SubMat.m_lpTextures[0] == NULL || nVSVersion <= 1 || nPSVersion <= 1)
                {
                    SubMat.m_dwPassID = PASS_SkinShader_old;
                }
                else if ((SubMat.m_dwOption & MATERIAL_OPTION_SPEC) && (SubMat.m_dwOption & MATERIAL_OPTION_EMAP))
                {
                    SubMat.m_dwPassID = PASS_SkinShader_all;
                }
                else if (SubMat.m_dwOption & MATERIAL_OPTION_SPEC)
                {
                    SubMat.m_dwPassID = PASS_SkinShader_spec;
                }
                else if (SubMat.m_dwOption & MATERIAL_OPTION_EMAP)
                {
                    SubMat.m_dwPassID = PASS_SkinShader_emap;
                }
                else
                {
                    SubMat.m_dwPassID = PASS_SkinShader_diff;
                }
            }
            break;
        case MAT_SHADER_TYPE_NORMAL_NOSKIN:
            break;
        }
    }
}

DWORD KG3DMaterial::GetShaderType()
{
    return m_ShaderType;
}

HRESULT KG3DMaterial::SetSubsetTexture(unsigned int uSubsetID,
                                       unsigned int uStageID,
                                       KG3DTexture* pTexture)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(pTexture);
    KG_PROCESS_ERROR(m_pMaterials);
    KG_PROCESS_ERROR(uSubsetID < m_dwNumMaterials);
    KG_PROCESS_ERROR(uStageID < KG3DMaterialSubset::cNumID);

    SAFE_RELEASE(m_pMaterials[uSubsetID].m_lpTextures[uStageID]);

    m_pMaterials[uSubsetID].m_lpTextures[uStageID] = pTexture;
    m_pMaterials[uSubsetID].m_scTextureName[uStageID] = pTexture->m_scName;

    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DMaterial::CreateSubset(DWORD dwNumSubset)
{
    HRESULT hr = E_FAIL;
    if (m_pMaterials != NULL)
    {
        for (size_t i = 0; i < m_dwNumMaterials; i++)
            if (g_pSubMatLast == (&(m_pMaterials[i])))
                g_pSubMatLast = NULL;
    }
    SAFE_DELETE_ARRAY(m_pMaterials);
    m_dwNumMaterials = 0;

    KG_PROCESS_ERROR(dwNumSubset > 0);

    m_pMaterials = new KG3DMaterialSubset[dwNumSubset];
    KG_PROCESS_ERROR(m_pMaterials);

    m_dwNumMaterials = dwNumSubset;

    hr = S_OK;
Exit0:
    return hr;
}
HRESULT KG3DMaterial::GetName(LPCTSTR *ppszName)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppszName);

    *ppszName = m_scName.c_str();

    hResult = S_OK;
Exit0:
    return hResult;
}



HRESULT KG3DMaterial::GetMaterialCount(DWORD *pdwCount)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwCount);

    *pdwCount = m_dwNumMaterials;

    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DMaterial::GetSubMaterialItemAlphaOp(DWORD dwIndex, _MtlAlphaOperation **ppAlphaOp)
{
    HRESULT hResult = E_FAIL;

    KGLOG_PROCESS_ERROR(ppAlphaOp);
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

    *ppAlphaOp = m_pMaterials[dwIndex].GetAlphaOperation();

    hResult = S_OK;
Exit0:
    return hResult;
}

static void ColorRef2Value(COLORREF rgb, D3DCOLORVALUE& Color)
{
    Color.a = ((rgb & 0xff000000)>>24) / 255.0f;
    Color.r = (rgb & 0x000000ff) / 255.0f;
    Color.g = ((rgb & 0x0000ff00)>>8) / 255.0f;
    Color.b = ((rgb & 0x00ff0000)>>16) / 255.0f;
}

HRESULT KG3DMaterial::SetSubMaterialColorRef2Value(
    DWORD dwIndex,
    DWORD dwAmbientColor,
    DWORD dwDiffuseColor,
    DWORD dwEmissive,
    DWORD dwSpecular
)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

    KG3DMaterial::KG3DMaterialSubset& SubMat = m_pMaterials[dwIndex];

    ColorRef2Value(dwAmbientColor, SubMat.m_sMaterial9.Ambient);
    ColorRef2Value(dwDiffuseColor, SubMat.m_sMaterial9.Diffuse);
    ColorRef2Value(dwEmissive, SubMat.m_sMaterial9.Emissive);
    ColorRef2Value(dwSpecular, SubMat.m_sMaterial9.Specular);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMaterial::GetSubMaterialOption(DWORD dwIndex, DWORD *pdwOption)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(pdwOption);
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

    *pdwOption = m_pMaterials[dwIndex].m_dwOption;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMaterial::SetSubMaterialOption(DWORD dwIndex, DWORD dwOption)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);

    m_pMaterials[dwIndex].m_dwOption = dwOption;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DMaterial::GetSubMaterial(DWORD dwIndex, PVOID *ppSubMat)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
    KGLOG_PROCESS_ERROR(ppSubMat);

    *ppSubMat = static_cast<PVOID>(&(m_pMaterials[dwIndex]));

    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DMaterial::GetSubMaterialTextureID(DWORD dwIndex, DWORD *pdwTextureIDs)
{
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(dwIndex < m_dwNumMaterials);
    KGLOG_PROCESS_ERROR(pdwTextureIDs);

    for (int i=0;i<8;i++)
    {
        if(m_pMaterials[dwIndex].m_lpTextures[i])
        {
            pdwTextureIDs[i] = m_pMaterials[dwIndex].m_lpTextures[i]->GetID();
        }
        else
        {
            pdwTextureIDs[i] = 0xFFFFFFFF;
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

BOOL KG3DMaterial::IsResourceReady(BOOL bTestTexture)
{
    BOOL bReady = IsLoaded();
    KG3DTexture* pTexture = NULL;

    if(!bReady)
        return bReady;

	if(bTestTexture)
	{
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			KG3DMaterial::KG3DMaterialSubset& SubMat = m_pMaterials[i];
			for(int i = 0; i < 1; i++)
			{
				pTexture = SubMat.m_lpTextures[i];
				if (pTexture)
				{
					if(!pTexture->IsResourceReady())
					{
						bReady = FALSE;
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

    return bReady;
}

HRESULT KG3DMaterial::RefreshOption(unsigned uOption)
{
    HRESULT hResult  = E_FAIL;

    m_uOption = uOption;

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DMaterial::RemoveTexture(DWORD dwSubsetIndex,
                                    DWORD dwIndex)
{
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(m_pMaterials);
    KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);
    KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];

    KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);

    SAFE_RELEASE(Subset.m_lpTextures[dwIndex]);
    Subset.m_scTextureName[dwIndex].clear();

    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DMaterial::ChangeTexture(DWORD dwSubsetIndex,
                                    DWORD dwIndex,
                                    LPCSTR strNewTextureFileName)
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hResult  = E_FAIL;
    KG3DTexture *pNewTexture = NULL;
    char strRelativePath[MAX_PATH];
    KG_PROCESS_ERROR(m_pMaterials);
    KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);
    KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];

    KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);

    //如果有旧的贴图， 把旧的清掉
    SAFE_RELEASE(Subset.m_lpTextures[dwIndex]);
    Subset.m_scTextureName[dwIndex].clear();
    Subset.m_scTextureType[dwIndex].clear();

    //加载新的贴图
    g_pFileFinder->GetRelativePath(strNewTextureFileName,
        strRelativePath,
        MAX_PATH);

    hResult = g_cTextureTable.LoadResourceFromFile(strRelativePath,
        0,
        0,
        (IKG3DResourceBase**)&pNewTexture);
    if (FAILED(hResult))
    {
        KG_PROCESS_ERROR(dwIndex == 0);
        KG_PROCESS_ERROR(m_pstrTextureName);
        hResult = g_cTextureTable.LoadResourceFromFile(m_pstrTextureName[dwSubsetIndex].c_str(),
            0,
            0,
            (IKG3DResourceBase**)&pNewTexture);
        KG_COM_PROCESS_ERROR(hResult);

        Subset.m_lpTextures[dwIndex] = pNewTexture;
        Subset.m_scTextureName[dwIndex] = m_pstrTextureName[dwSubsetIndex];
    }
    else
    {
        Subset.m_lpTextures[dwIndex] = pNewTexture;
        Subset.m_scTextureName[dwIndex] = strRelativePath;
    }
    Subset.m_scTextureType[dwIndex] = TEXTURE_TYPE_DIFFUSE;
    
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DMaterial::GetTexturePathName(
    DWORD dwSubsetIndex,
    DWORD dwIndex,
    LPSTR strBuffer,
    DWORD dwSize,
    TEXTURE_LOAD_STATE *peLoadState
)
{
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(peLoadState);
    *peLoadState = TEXTURE_LOAD_UNKNOWN_ERROR;

    KG_PROCESS_ERROR(strBuffer);
    KG_PROCESS_ERROR(dwSize);
    KG_PROCESS_ERROR(m_pMaterials);
    strBuffer[0] = '\0';

    KG_PROCESS_ERROR(dwSubsetIndex < m_dwNumMaterials);

    KG3DMaterialSubset &Subset = m_pMaterials[dwSubsetIndex];
    KG_PROCESS_ERROR(dwIndex < KG3DMaterialSubset::cNumID);

    if (Subset.m_scTextureName[dwIndex].empty())
        *peLoadState = TEXTURE_LOAD_NOT_EXIST;
    else if (Subset.m_lpTextures[dwIndex])
        *peLoadState = TEXTURE_LOAD_SUCCESS;
    else
        *peLoadState = TEXTURE_LOAD_FAILED;

    strcpy_s(strBuffer, dwSize, Subset.m_scTextureName[dwIndex].c_str());
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}


KG3DMaterialDetail::KG3DMaterialDetail()
{
    m_nIndex = -1;

    m_cMainColor.r = m_cMainColor.g = m_cMainColor.b = 1;
	m_cMainColor1.r = m_cMainColor1.g = m_cMainColor1.b = 1;

    m_cMainColor.a = m_cMainColor1.a = 0;

    m_fMainColorMin = 0;
    m_fMainColorMax = 1;
    m_fMainColorScale = 1;
    m_fEnv = 1;
    m_fSpecular = 0.2F;

    m_szDetail0FileName = "";
    m_szDetail1FileName = "";

    m_lpDetailTexture0 = NULL;
    m_lpDetailTexture1 = NULL;
    m_lpChannelTexture = NULL;
	m_lpFlowTexture    = NULL;

    m_fDetail0UVScale = 1;
    m_fDetail1UVScale = 1;
    m_fDetail0ColorScale = 1;
    m_fDetail1ColorScale = 1;

    m_fDetail0Env      = 0;
    m_fDetail0Specular = 0;
    m_fDetail1Env      = 0;
    m_fDetail1Specular = 0;

    m_cDetial0Color = m_cMainColor;
    m_cDetial1Color = m_cMainColor;

	m_Flow_Color.a = m_Flow_Color.r = m_Flow_Color.g = m_Flow_Color.b = 1;
	m_fFlow_ColorScale = 1;
	m_fFlow_UVScale    = 1;
	m_vFlow_Speed      = D3DXVECTOR2(1,1);

    m_szTextrueFileName = "";

    memset(m_SubSetDetail, NULL, sizeof(m_SubSetDetail));
}

KG3DMaterialDetail::~KG3DMaterialDetail()
{
    UnInit();
}

HRESULT KG3DMaterial::GetMaterilDetail(int nIndex,KG3DMaterialDetail* pDest)
{
    HRESULT hResult = E_FAIL;
    BOOL bHasSubDetail = FALSE;
    int nVecIndex = FindIndex(nIndex);
    KG_PROCESS_ERROR(nVecIndex != -1);
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[nVecIndex];
		KG_PROCESS_ERROR(pDetail);

        SetMaterialDetail(pDest, pDetail);
    
        KG_PROCESS_ERROR(m_dwNumMaterials <= MAX_SUBSET_COUNT + 1);
        for (DWORD i = 0; i < m_dwNumMaterials - 1; i++)
        {
            KG3DMaterialDetail* pSubDetail = pDetail->m_SubSetDetail[i];
            KG3DMaterialDetail* pSubDest   = pDest->m_SubSetDetail[i];
            if (!pSubDetail)
            {
                SAFE_DELETE(pDest->m_SubSetDetail[i]);
                continue;
            }
            if (!pSubDest)
            {
                pSubDest = new KG3DMaterialDetail;
                KG_PROCESS_ERROR(pSubDest);

                pDest->m_SubSetDetail[i] = pSubDest;
            }

            SetMaterialDetail(pSubDest, pSubDetail);
            pSubDest->m_szChannelFileName = pSubDetail->m_szChannelFileName;
            pSubDest->LoadTextures();

            bHasSubDetail = TRUE;
        }
    }

    hResult = S_OK;
Exit0:
    m_bDetailWholeColorCast = !bHasSubDetail;
    return hResult;
}

HRESULT KG3DMaterial::SetMaterialDetail(KG3DMaterialDetail* pDest, KG3DMaterialDetail* pSource)
{
    HRESULT hResult = E_FAIL;

    KG_PROCESS_ERROR(pDest);
    KG_PROCESS_ERROR(pSource);

    pDest->m_fMainColorMax   = pSource->m_fMainColorMax;
    pDest->m_fMainColorMin   = pSource->m_fMainColorMin;
    pDest->m_fMainColorScale = pSource->m_fMainColorScale;
    pDest->m_cMainColor      = pSource->m_cMainColor;
    pDest->m_cMainColor1     = pSource->m_cMainColor1;
    pDest->m_fEnv            = pSource->m_fEnv;
    pDest->m_fSpecular       = pSource->m_fSpecular;

    pDest->m_fDetail0ColorScale = pSource->m_fDetail0ColorScale;
    pDest->m_fDetail0UVScale    = pSource->m_fDetail0UVScale;
    pDest->m_cDetial0Color      = pSource->m_cDetial0Color;
    pDest->m_szDetail0FileName  = pSource->m_szDetail0FileName;
    pDest->m_fDetail0Env        = pSource->m_fDetail0Env;
    pDest->m_fDetail0Specular   = pSource->m_fDetail0Specular;

    pDest->m_fDetail1ColorScale = pSource->m_fDetail1ColorScale;
    pDest->m_fDetail1UVScale    = pSource->m_fDetail1UVScale;
    pDest->m_cDetial1Color      = pSource->m_cDetial1Color;
    pDest->m_szDetail1FileName  = pSource->m_szDetail1FileName;
    pDest->m_fDetail1Env        = pSource->m_fDetail1Env;
    pDest->m_fDetail1Specular   = pSource->m_fDetail1Specular;

    pDest->m_szName = pSource->m_szName;
    pDest->m_szTextrueFileName = pSource->m_szTextrueFileName;

	pDest->m_fFlow_ColorScale = pSource->m_fFlow_ColorScale;
	pDest->m_Flow_Color = pSource->m_Flow_Color;
	pDest->m_fFlow_UVScale = pSource->m_fFlow_UVScale;
	pDest->m_vFlow_Speed = pSource->m_vFlow_Speed;
	pDest->m_szFlow_Texture = pSource->m_szFlow_Texture;

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DMaterial::GetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float* pfEnv,float* pfSpecular,
                                 float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,D3DCOLORVALUE* pBodyColor1,LPSTR pChannelFile,
                                 float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
                                 float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File,
								 D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture)
{
    int nVecIndex = FindIndex(nDetailType);
    KG_PROCESS_ERROR(nVecIndex != -1);

    KG_PROCESS_ERROR(nSubSetID < (int)m_dwNumMaterials);
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[nVecIndex];
		KG_PROCESS_ERROR(pDetail);

        KG3DMaterialDetail* pSubSetDetail = pDetail->GetSubSetDetail(m_bDetailWholeColorCast, nSubSetID);
        KG_PROCESS_ERROR(pSubSetDetail);

        (*pBodyMax)   = pSubSetDetail->m_fMainColorMax;
        (*pBodyMin)   = pSubSetDetail->m_fMainColorMin;
        (*pBodyScale) = pSubSetDetail->m_fMainColorScale;
        (*pBodyColor) = pSubSetDetail->m_cMainColor;
		(*pBodyColor1)= pSubSetDetail->m_cMainColor1;
        (*pfEnv)      = pSubSetDetail->m_fEnv;
        (*pfSpecular) = pSubSetDetail->m_fSpecular;
        wsprintf(pChannelFile,"%s",pSubSetDetail->m_szChannelFileName.c_str());

        (*pDetail0_ColorScale) = pSubSetDetail->m_fDetail0ColorScale;
        (*pDetail0_UVScale)    = pSubSetDetail->m_fDetail0UVScale;
        (*pDetail0_Color)      = pSubSetDetail->m_cDetial0Color;
        (*pfDetail0_Env)       = pSubSetDetail->m_fDetail0Env;
        (*pfDetail0_Specular)  = pSubSetDetail->m_fDetail0Specular;
        wsprintf(pDetail0_File,"%s",pSubSetDetail->m_szDetail0FileName.c_str());

        (*pDetail1_ColorScale) = pSubSetDetail->m_fDetail1ColorScale;
        (*pDetail1_UVScale)    = pSubSetDetail->m_fDetail1UVScale;
        (*pDetail1_Color)      = pSubSetDetail->m_cDetial1Color;
        (*pfDetail1_Env)       = pSubSetDetail->m_fDetail1Env;
        (*pfDetail1_Specular)  = pSubSetDetail->m_fDetail1Specular;
        wsprintf(pDetail1_File,"%s",pSubSetDetail->m_szDetail1FileName.c_str());

        wsprintf(pszName,"%s",pDetail->m_szName.c_str());
        if (pDetail->m_szTextrueFileName != "")
        {
            wsprintf(pszTextureFileName, "%s", pDetail->m_szTextrueFileName.c_str());
        }
        else
        {
            wsprintf(pszTextureFileName, "%s", m_strOrgTextureFilePath.c_str());
        }

		(*pFlow_Color) = pSubSetDetail->m_Flow_Color;
		(*pfFlow_ColorScale) = pSubSetDetail->m_fFlow_ColorScale;
		(*pfFlow_UVScale) = pSubSetDetail->m_fFlow_UVScale;
		(*pvSpeed) = pSubSetDetail->m_vFlow_Speed;
		wsprintf(pFlowTexture,"%s",pSubSetDetail->m_szFlow_Texture.c_str());

        return S_OK;
    }

Exit0:
    return E_FAIL;
}

HRESULT KG3DMaterial::SetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float fEnv,float fSpecular,
                                 float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,D3DCOLORVALUE vBodyColor1,LPSTR pChannelFile,
                                 float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
                                 float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File,
								 D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture)
{
    int nVecIndex = FindIndex(nDetailType);
    KG_PROCESS_ERROR(nVecIndex != -1);
    
    KG_PROCESS_ERROR(nSubSetID < (int)m_dwNumMaterials);
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[nVecIndex];
		KG_PROCESS_ERROR(pDetail);

        KG3DMaterialDetail* pSubSetDetail = pDetail->GetSubSetDetail(m_bDetailWholeColorCast, nSubSetID);
        KG_PROCESS_ERROR(pSubSetDetail);

        pSubSetDetail->m_fMainColorMax   =  fBodyMax  ;
        pSubSetDetail->m_fMainColorMin   =  fBodyMin  ;
        pSubSetDetail->m_fMainColorScale =  fBodyScale;
        pSubSetDetail->m_cMainColor      =  vBodyColor;
		pSubSetDetail->m_cMainColor1     =  vBodyColor1;
        pSubSetDetail->m_fEnv            =  fEnv      ;
        pSubSetDetail->m_fSpecular       =  fSpecular ;
        pSubSetDetail->m_szChannelFileName  = pChannelFile;

        pSubSetDetail->m_fDetail0ColorScale = fDetail0_ColorScale;
        pSubSetDetail->m_fDetail0UVScale    = fDetail0_UVScale   ;
        pSubSetDetail->m_cDetial0Color      = vDetail0_Color     ;
        pSubSetDetail->m_fDetail0Env        = fDetail0_Env       ;
        pSubSetDetail->m_fDetail0Specular   = fDetail0_Specular  ;
        pSubSetDetail->m_szDetail0FileName  = pDetail0_File;

        pSubSetDetail->m_fDetail1ColorScale = fDetail1_ColorScale;
        pSubSetDetail->m_fDetail1UVScale    = fDetail1_UVScale   ;
        pSubSetDetail->m_cDetial1Color      = vDetail1_Color     ;
        pSubSetDetail->m_fDetail1Env        = fDetail1_Env       ;
        pSubSetDetail->m_fDetail1Specular   = fDetail1_Specular  ;
        pSubSetDetail->m_szDetail1FileName  = pDetail1_File;

        pDetail->m_szName = pszName;
        pDetail->m_szTextrueFileName = pszTextureFileName;

		pSubSetDetail->m_Flow_Color = Flow_Color;
		pSubSetDetail->m_fFlow_ColorScale = fFlow_ColorScale;
		pSubSetDetail->m_fFlow_UVScale = fFlow_UVScale;
		pSubSetDetail->m_vFlow_Speed = vSpeed;
		pSubSetDetail->m_szFlow_Texture= pFlowTexture;

            
        return S_OK;
    }
Exit0:
    return E_FAIL;
}

HRESULT KG3DMaterialDetail::LoadFormIni(IIniFile* pFile,LPSTR SectionName)
{
    char szName   [MAX_PATH];
    char szChannel[MAX_PATH];
    char szDetail0[MAX_PATH];
    char szDetail1[MAX_PATH];
    char szTextureFileName[MAX_PATH];
	char szFlow_Texture[MAX_PATH];

    pFile->GetMultiFloat(SectionName,"MainColor"        ,(float*)&m_cMainColor,4);
	pFile->GetMultiFloat(SectionName,"MainColor1"        ,(float*)&m_cMainColor1,4);
    pFile->GetFloat     (SectionName,"MainColorMin"     ,0                    ,&m_fMainColorMin);
    pFile->GetFloat     (SectionName,"MainColorMax"     ,1                    ,&m_fMainColorMax);
    pFile->GetFloat     (SectionName,"MainColorScale"   ,1                    ,&m_fMainColorScale);
    pFile->GetString    (SectionName,"ChannelFileName"  ,""                   ,szChannel,MAX_PATH);
    pFile->GetFloat     (SectionName,"Env"              ,1                    ,&m_fEnv           );
    pFile->GetFloat     (SectionName,"Specular"         ,1                    ,&m_fSpecular      );

    pFile->GetFloat     (SectionName,"Detail0UVScale"   ,0                    ,&m_fDetail0UVScale);
    pFile->GetFloat     (SectionName,"Detail0ColorScale",0                    ,&m_fDetail0ColorScale);
    pFile->GetFloat     (SectionName,"Detail0Env"       ,0                    ,&m_fDetail0Env);
    pFile->GetFloat     (SectionName,"Detail0Specular"  ,0                    ,&m_fDetail0Specular);
    pFile->GetMultiFloat(SectionName,"Detial0Color"     ,(float*)&m_cDetial0Color,4);
    pFile->GetString    (SectionName,"Detail0FileName"  ,""                   ,szDetail0,MAX_PATH);

    pFile->GetFloat     (SectionName,"Detail1UVScale"   ,0                    ,&m_fDetail1UVScale);
    pFile->GetFloat     (SectionName,"Detail1ColorScale",0                    ,&m_fDetail1ColorScale);
    pFile->GetFloat     (SectionName,"Detail1Env"       ,0                    ,&m_fDetail1Env);
    pFile->GetFloat     (SectionName,"Detail1Specular"  ,0                    ,&m_fDetail1Specular);
    pFile->GetMultiFloat(SectionName,"Detial1Color"     ,(float*)&m_cDetial1Color,4);
    pFile->GetString    (SectionName,"Detail1FileName"  ,""                   ,szDetail1,MAX_PATH);

    pFile->GetString    (SectionName,"Name"             ,""                   ,szName,MAX_PATH);
    pFile->GetString    (SectionName,"TextureFileName"  ,""                   ,szTextureFileName, MAX_PATH);

	pFile->GetMultiFloat(SectionName,"Flow_Color"     ,(float*)&m_Flow_Color,4         );
	pFile->GetFloat     (SectionName,"Flow_ColorScale",0        ,&m_fFlow_ColorScale   );
	pFile->GetFloat     (SectionName,"Flow_UVScale"   ,0        ,&m_fFlow_UVScale      );
	pFile->GetMultiFloat(SectionName,"Flow_Speed"     ,(float*)&m_vFlow_Speed,2        );
	pFile->GetString    (SectionName,"Flow_Texture"   , "",szFlow_Texture  ,MAX_PATH   );

    m_szChannelFileName = szChannel;
    m_szDetail0FileName = szDetail0;
    m_szDetail1FileName = szDetail1;
    m_szName            = szName;
    m_szTextrueFileName = szTextureFileName;
	m_szFlow_Texture    = szFlow_Texture;

    return S_OK;
}


HRESULT KG3DMaterialDetail::SaveDefineToIni(IIniFile* pFile,LPSTR SectionName)
{
    char szName   [MAX_PATH];
    char szChannel[MAX_PATH];
    char szDetail0[MAX_PATH];
    char szDetail1[MAX_PATH];
    char szTextureFileName[MAX_PATH];
	char szFlow_Texture[MAX_PATH];

    wsprintf(szChannel,"%s",m_szChannelFileName.c_str());
    wsprintf(szDetail0,"%s",m_szDetail0FileName.c_str());
    wsprintf(szDetail1,"%s",m_szDetail1FileName.c_str());
    wsprintf(szName   ,"%s",m_szName.c_str()           );
    wsprintf(szTextureFileName, "%s", m_szTextrueFileName.c_str());
	wsprintf(szFlow_Texture, "%s", m_szFlow_Texture.c_str());

    pFile->WriteMultiFloat(SectionName,"MainColor"        ,(float*)&m_cMainColor,4   );
	pFile->WriteMultiFloat(SectionName,"MainColor1"        ,(float*)&m_cMainColor1,4   );
    pFile->WriteFloat     (SectionName,"MainColorMin"     ,m_fMainColorMin           );
    pFile->WriteFloat     (SectionName,"MainColorMax"     ,m_fMainColorMax           );
    pFile->WriteFloat     (SectionName,"MainColorScale"   ,m_fMainColorScale         );
    pFile->WriteString    (SectionName,"ChannelFileName"  ,szChannel                 );
    pFile->WriteFloat     (SectionName,"Env"              ,m_fEnv                   );
    pFile->WriteFloat     (SectionName,"Specular"         ,m_fSpecular              );

    pFile->WriteFloat     (SectionName,"Detail0UVScale"   ,m_fDetail0UVScale         );
    pFile->WriteFloat     (SectionName,"Detail0ColorScale",m_fDetail0ColorScale      );
    pFile->WriteFloat     (SectionName,"Detail0Env"       ,m_fDetail0Env             );
    pFile->WriteFloat     (SectionName,"Detail0Specular"  ,m_fDetail0Specular        );
    pFile->WriteMultiFloat(SectionName,"Detial0Color"     ,(float*)&m_cDetial0Color,4);
    pFile->WriteString    (SectionName,"Detail0FileName"  ,szDetail0                 );

    pFile->WriteFloat     (SectionName,"Detail1UVScale"   ,m_fDetail1UVScale         );
    pFile->WriteFloat     (SectionName,"Detail1ColorScale",m_fDetail1ColorScale      );
    pFile->WriteFloat     (SectionName,"Detail1Env"       ,m_fDetail1Env             );
    pFile->WriteFloat     (SectionName,"Detail1Specular"  ,m_fDetail1Specular        );
    pFile->WriteMultiFloat(SectionName,"Detial1Color"     ,(float*)&m_cDetial1Color,4);
    pFile->WriteString    (SectionName,"Detail1FileName"  ,szDetail1                 );
    pFile->WriteString    (SectionName,"Name"             ,szName                    );
    pFile->WriteString    (SectionName,"TextureFileName"  ,szTextureFileName         );

	pFile->WriteMultiFloat(SectionName,"Flow_Color"  ,(float*)&m_Flow_Color,4);
	pFile->WriteFloat     (SectionName,"Flow_ColorScale",m_fFlow_ColorScale);
	pFile->WriteFloat     (SectionName,"Flow_UVScale"   ,m_fFlow_UVScale   );
	pFile->WriteMultiFloat(SectionName,"Flow_Speed"  ,(float*)&m_vFlow_Speed,2);
	pFile->WriteString    (SectionName,"Flow_Texture",szFlow_Texture);

    return S_OK;
}

HRESULT KG3DMaterialDetail::LoadTextures()
{
    HRESULT hr = E_FAIL;

    KG3DTexture* pDetailTexture0 = NULL;
    KG3DTexture* pDetailTexture1 = NULL;
    KG3DTexture* pChannelTexture = NULL;
	KG3DTexture* pFlowTexture    = NULL;

    if(m_szChannelFileName.size())
    {
        hr = g_cTextureTable.LoadResourceFromFile(m_szChannelFileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pChannelTexture);
        if(SUCCEEDED(hr))
        {
            SAFE_RELEASE(m_lpChannelTexture);
            m_lpChannelTexture = pChannelTexture;
        }
    }

    if(m_szDetail0FileName.size())
    {
        hr = g_cTextureTable.LoadResourceFromFile(m_szDetail0FileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pDetailTexture0);
        if(SUCCEEDED(hr))
        {
            SAFE_RELEASE(m_lpDetailTexture0);
            m_lpDetailTexture0 = pDetailTexture0;
        }
    }
	else
	{
		SAFE_RELEASE(m_lpDetailTexture0);
	}

    if(m_szDetail1FileName.size())
    {
        hr = g_cTextureTable.LoadResourceFromFile(m_szDetail1FileName.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pDetailTexture1);
        if(SUCCEEDED(hr))
        {
            SAFE_RELEASE(m_lpDetailTexture1);
            m_lpDetailTexture1 = pDetailTexture1;
        }
    }
	else
	{
		SAFE_RELEASE(m_lpDetailTexture1)
	}

	if(m_szFlow_Texture.size())
	{
		hr = g_cTextureTable.LoadResourceFromFile(m_szFlow_Texture.c_str(),0,TEXOP_RELOAD,(IKG3DResourceBase**)&pFlowTexture);
		if(SUCCEEDED(hr))
		{
			SAFE_RELEASE(m_lpFlowTexture);
			m_lpFlowTexture = pFlowTexture;
		}
	}
	else
	{
		SAFE_RELEASE(m_lpFlowTexture)
	}

    return S_OK;
}


int KG3DMaterial::GetMaterialDetailDefineCount()
{
    return (int)m_vecDetailDefine.size();
}

HRESULT KG3DMaterial::AddMaterialDetailDefine(int nIndex)
{
    int nVecIndex = FindIndex(nIndex);
    KG_PROCESS_ERROR(nVecIndex == -1);

	KG3DMaterialDetail* pdetail = new KG3DMaterialDetail;
	KGLOG_PROCESS_ERROR(pdetail);
    pdetail->m_nIndex = nIndex;
	m_vecDetailDefine.push_back(pdetail);
    
    m_nMaxMtlIndex = max(nIndex, m_nMaxMtlIndex);

    return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMaterial::DeleteMaterialDetailDefine(int nIndex)
{
    int nVecIndex = FindIndex(nIndex);
    KG_PROCESS_ERROR(nVecIndex != -1);

	SAFE_DELETE(m_vecDetailDefine[nVecIndex]);
    m_vecDetailDefine.erase(m_vecDetailDefine.begin() + nVecIndex);
    return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DMaterial::SaveMaterialDetails(LPSTR pFileName)
{
    char szSection[MAX_PATH];
    IIniFile* pFile = NULL;

    KGLOG_PROCESS_ERROR(pFileName);
    
    pFile = g_OpenIniFile(pFileName, true, true);
    KG_PROCESS_ERROR(pFile);

    int nCount = (int)m_vecDetailDefine.size();

    pFile->WriteInteger("Main","DefineCount",nCount);
    pFile->WriteString("Main", "OrgTextureFileName", m_strOrgTextureFilePath.c_str());

    for (int i=0;i<nCount;i++)
    {
		BOOL bExist = FALSE;
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[i];
		
        if (!pDetail)
            continue;
		
        wsprintf(szSection,"DetailDefine%d",pDetail->m_nIndex);

        bExist = TRUE;
		pFile->WriteInteger(szSection,"bExist",bExist);
        pDetail->SaveDefineToIni(pFile,szSection);

        TCHAR szSubSection[MAX_PATH];
        for (int nID = 0; nID < MAX_SUBSET_COUNT; nID++)
        {
            KG3DMaterialDetail* pSubDetail = pDetail->m_SubSetDetail[nID];
            if (!pSubDetail)
                continue;
    
            wsprintf(szSubSection, "%s_%d", szSection, nID);
            pSubDetail->SaveDefineToIni(pFile, szSubSection);
        }
    }

    pFile->Save(pFileName);
    SAFE_RELEASE(pFile);
    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KG3DMaterial::LoadMaterialDetails(LPSTR pFileName)
{
    HRESULT hResult = E_FAIL;
    int nRetCode = false;
    char szSection[MAX_PATH];
    char szPreSection[MAX_PATH];
    char szSubSection[MAX_PATH];
    char szSectionName[] = {"DetailDefine"};
    int nLength = (int)strlen(szSectionName);
    int i       = 0;
	IIniFile* pFile = NULL;
    KG3DMaterialDetail* pDetail = NULL;
    TCHAR szOrgTextureFileName[MAX_PATH];

    KGLOG_PROCESS_ERROR(pFileName);
    pFile = g_OpenIniFile(pFileName);
    KG_PROCESS_ERROR(pFile);

    int nCount = 0;
    pFile->GetInteger("Main","DefineCount",0,&nCount);

    wsprintf(szPreSection, "Main");
    if (m_strOrgTextureFilePath == "")
    {
        pFile->GetString(szPreSection, "OrgTextureFileName", "", szOrgTextureFileName, MAX_PATH);
        if (strcmp(szOrgTextureFileName, ""))
        {
            FILE* pFile = NULL;
            pFile = fopen(szOrgTextureFileName, "r+");
            if (pFile)
            {
                m_strOrgTextureFilePath = szOrgTextureFileName;
                fclose(pFile);
            }
            else
            {
                m_strOrgTextureFilePath = m_pstrTextureName[0];
            }
        }
        else
        {
            m_strOrgTextureFilePath = m_pstrTextureName[0];
        }
    }
    
    while (true)
    {
        nRetCode = pFile->GetNextSection(szPreSection, szSection);
        if (!nRetCode)
            break;

        memcpy(szPreSection, szSection, sizeof(szSection));
        int nSectionLength = (int)strlen(szSection);

        if (nSectionLength <= nLength)
            continue;

        memcpy(szSubSection, szSection, nLength);
        szSubSection[nLength] = '\0';
        if (strcmp(szSubSection, szSectionName))
            continue;

        int nIndex = 0;
        for (i = nLength; i < nSectionLength; i++)
        {
            if (szSection[i] >= '0' && szSection[i] <= '9')
                nIndex = nIndex * 10 + (szSection[i] - '0');
            else
                break;
        }
        if (i < nSectionLength)
            continue;

        BOOL bExist = FALSE;
        pFile->GetInteger(szSection, "bExist", FALSE, &bExist);
        if (!bExist)
        {
            TCHAR szName[MAX_PATH];
            pFile->GetString(szSection, "Name", "", szName, MAX_PATH);

            if (_strcmpi(szName, "") != 0)
                bExist = TRUE;
        }

        if (bExist)
        {
            pDetail = new KG3DMaterialDetail;
            KGLOG_PROCESS_ERROR(pDetail);

            pDetail->m_nIndex = nIndex;
            pDetail->LoadFormIni(pFile, szSection);
            
            TCHAR szSubSection[MAX_PATH];
            int   nSectionExist = false;
            for (int nID = 0; nID < MAX_SUBSET_COUNT; nID++)
            {
                wsprintf(szSubSection, "%s_%d", szSection, nID);
                nSectionExist = pFile->IsSectionExist(szSubSection);
                if (!nSectionExist)
                    continue;

                KG3DMaterialDetail* pSubDetail = new KG3DMaterialDetail;
                KG_PROCESS_ERROR(pSubDetail);

                pSubDetail->LoadFormIni(pFile, szSubSection);
                pDetail->m_SubSetDetail[nID] = pSubDetail;
            }
            m_vecDetailDefine.push_back(pDetail);
            pDetail = NULL;

            m_nMaxMtlIndex = max(m_nMaxMtlIndex, nIndex);
        }
    }

    hResult = S_OK;
Exit0:
    SAFE_RELEASE(pFile);
    if (FAILED(hResult))
    {
        if (pDetail)
            SAFE_DELETE(pDetail);
        for (size_t i = 0; i < m_vecDetailDefine.size(); i++)
        {
            SAFE_DELETE(m_vecDetailDefine[i]);
        }
        m_vecDetailDefine.clear();
    }
    return hResult;
}


BOOL KG3DMaterialDetail::IsResourceReady()
{
    BOOL bLoad = TRUE;

    if(m_lpChannelTexture)
        bLoad &= m_lpChannelTexture->IsResourceReady();

    if(m_lpDetailTexture0)
        bLoad &= m_lpDetailTexture0->IsResourceReady();

    if(m_lpDetailTexture1)
        bLoad &= m_lpDetailTexture1->IsResourceReady();

    return bLoad;
}

KG3DMaterialDetail* KG3DMaterialDetail::GetSubSetDetail(BOOL bWholeColorCast, int nID)
{
    KG3DMaterialDetail* pDetail = NULL;

    if (bWholeColorCast || nID == 0)
    {
        pDetail = this;
    }

    else
    {
        KG_PROCESS_ERROR(nID <= MAX_SUBSET_COUNT);
        pDetail = m_SubSetDetail[nID - 1];
        if (pDetail == NULL)
        {
            pDetail = new KG3DMaterialDetail;
            KG_PROCESS_ERROR(pDetail);
            
            m_SubSetDetail[nID - 1] = pDetail;
        }
    }

Exit0:
    return pDetail;
}

void KG3DMaterialDetail::UnInit()
{
    SAFE_RELEASE(m_lpDetailTexture0);
    SAFE_RELEASE(m_lpDetailTexture1);
    SAFE_RELEASE(m_lpChannelTexture);
	SAFE_RELEASE(m_lpFlowTexture);

    for (int i = 0; i < MAX_SUBSET_COUNT; i++)
    {
        SAFE_DELETE(m_SubSetDetail[i]);
    }
}

int KG3DMaterial::FindIndex(int nIndex)
{
    int nResult = -1;
    for (int i = 0; i < (int)m_vecDetailDefine.size(); i++)
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[i];
        ASSERT(pDetail);
        if (pDetail->m_nIndex == nIndex)
        {
            nResult = i;
        }
    }

    return nResult;
}

BOOL KG3DMaterial::IsSubSetDetailExist(int nIndex)
{
    BOOL bResult = FALSE;

    int nVecIndex = FindIndex(nIndex);
    KG_PROCESS_ERROR(nVecIndex != -1);

    KG3DMaterialDetail* pDetail = m_vecDetailDefine[nVecIndex];
    KG_PROCESS_ERROR(pDetail);

    KG_PROCESS_ERROR(m_dwNumMaterials <= MAX_SUBSET_COUNT + 1);
    for (DWORD i = 0; i < m_dwNumMaterials - 1; i++)
    {
        if (pDetail->m_SubSetDetail[i])
        {
            bResult = TRUE;
            break;
        }
    }

Exit0:
    return bResult;
}

HRESULT KG3DMaterial::CopyTextureToFile(LPCSTR pszSourceFile, LPCSTR pszDestFile)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    KG3DTexture* pTexture = NULL;
    TCHAR szRelativePath[MAX_PATH];

    g_pFileFinder->GetRelativePath(pszSourceFile,
        szRelativePath,
        MAX_PATH);

    hRetCode = g_cTextureTable.LoadResourceFromFile(szRelativePath,
        0,
        0,
        (IKG3DResourceBase**)&pTexture);
    KG_COM_PROCESS_ERROR(hRetCode);
    KG_PROCESS_ERROR(pTexture);

    hRetCode = pTexture->SaveSysToFile(pszDestFile, 0);
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    SAFE_RELEASE(pTexture);

    return hResult;
}

HRESULT KG3DMaterial::MoveMaterialDetails(int nIndex)
{
    HRESULT hResult = E_FAIL;

    int nSize = (int)m_vecDetailDefine.size();
    for (int i = 0; i < nSize; i++)
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[i];
        KG_PROCESS_ERROR(pDetail);
        if ((pDetail->m_nIndex) > nIndex)
            (pDetail->m_nIndex)--;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

string KG3DMaterial::GetOrgTextureFileName()
{
    return m_strOrgTextureFilePath;
}

HRESULT KG3DMaterial::SetOrgTextureFileName(LPCSTR pszNewFileName)
{
    HRESULT hResult  = E_FAIL;
    TCHAR szRetFile[MAX_PATH];
    int nSize = (int)m_vecDetailDefine.size();

    ASSERT(g_pFileFinder);
    g_pFileFinder->GetRelativePath(pszNewFileName, szRetFile, MAX_PATH);
    
    for (int i = 0; i < nSize; i++)
    {
        KG3DMaterialDetail* pDetail = m_vecDetailDefine[i];
        KG_PROCESS_ERROR(pDetail);
        if (pDetail->m_szTextrueFileName == m_strOrgTextureFilePath)
        {
            pDetail->m_szTextrueFileName = szRetFile;
        }
    }

    m_strOrgTextureFilePath = szRetFile;

    hResult = S_OK;
Exit0:
    return hResult;
}