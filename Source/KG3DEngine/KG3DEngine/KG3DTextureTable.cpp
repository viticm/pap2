//#pragma warning(disable:4505)
#include "StdAfx.h"
#include "KG3DTextureTable.h"
#include "KG3DTextureAnimation.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DModel.h"
#include "KG3DFlashTexture.h"
#include "KGStringFunctions.h"
#include "kg3dscenetable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TKG3DResourcePool<KG3DDetail_Define>g_cDetailDefinePool;

KG3DTextureTable g_cTextureTable;
LPDIRECT3DCUBETEXTURE9 s_lpEnvMap = NULL;

static TypeInfo s_TextureTableTypes[] = {
	{ ".anitexture", TEXOP_ANIMATION, FALSE },
	{ ".swf", TEXOP_FLASH, FALSE }
};

TCHAR* KG3DTextureTable::s_strSpecialTextureFilePath[KG3DTextureTable::STI_COUNT] = {
	"",
	"data\\public\\errortexture.tga",
	"data\\public\\nulltex.tga",
	"",
	"data\\public\\ConverMap.tga"
};

KG3DTextureTable::enuSpecialTextureID KG3DTextureTable::s_SpecialTextureID[KG3DTextureTable::STI_COUNT] = 
{
	STID_INVALIDATE, 
	STID_ERROR, 
	STID_NULL, 
	STID_RENDERTARGET,
};


static TypeInfo s_TextureTableDefaultType = { "", TEXOP_COMMON, FALSE };

bool operator < (const USIZE& _Left, const USIZE& _Right)
{
    if (_Left.x <  _Right.x)
        return true; 

    if (_Left.x == _Right.x)
        return _Left.y < _Right.y;

    return false;

}

bool operator < (const _KG3DTEXTURE_DESC& l, const _KG3DTEXTURE_DESC& r)
{
	UINT ll = l.Format + l.Width*7 + l.Height + l.Usage + l.Pool + l.MipLevels;
	UINT rr = r.Format + r.Width*7 + r.Height + r.Usage + r.Pool + r.MipLevels;

	return ll < rr;
}

static void RemoveWarningC4505(ATL::CAxHostWindow* pA)
{
	IID a;
	if(pA) 
	{
		pA->QueryInterface(a, NULL);
		pA->AddRef();
		pA->Release();
	}
}

KG3DTexture* KG3DTextureTable::GetNullTexture()
{
	return m_SpecialTextures[STI_NULL];
}
KG3DTexture* KG3DTextureTable::GetConverTexture()
{
	return m_SpecialTextures[STI_CONVERMAP];
}

KG3DTextureTable::KG3DTextureTable(void)
{
	memset(m_SpecialTextures, 0, sizeof(KG3DTexture*) * STI_COUNT);
	RemoveWarningC4505(NULL);
	m_dwReleaseQueueSize = 80;

}

KG3DTextureTable::~KG3DTextureTable(void)
{
    std::map<USIZE, LPDIRECT3DTEXTURE9>::iterator i = m_mapChessboardTextures.begin();
    while (i != m_mapChessboardTextures.end())
    {
        SAFE_RELEASE(i->second);
        i++;
    }
	m_mapChessboardTextures.clear();

	SAFE_RELEASE(s_lpEnvMap);
}

LPDIRECT3DTEXTURE9 KG3DTextureTable::GetChessboardTexture(UINT uWidth, UINT uHeight)
{
    if (m_mapChessboardTextures.find(USIZE(uWidth, uHeight))!= m_mapChessboardTextures.end())
        return m_mapChessboardTextures[USIZE(uWidth, uHeight)];

    LPDIRECT3DTEXTURE9 pTexture = NULL;

    g_cGraphicsTool.CreateChessboardTexture(uWidth, uHeight, 0xFFFFFFFF, &pTexture);

    if (!pTexture)
        return NULL;

    m_mapChessboardTextures[USIZE(uWidth, uHeight)] = pTexture;

    return pTexture;

}

HRESULT KG3DTextureTable::GetIEKG3DTexture(IEKG3DTexture** ppTexture, DWORD dwID)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    KG3DTexture *pTexture = NULL;
	KGLOG_PROCESS_ERROR(ppTexture);
        
    hRetCode = GetResource(&pTexture, dwID);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    *ppTexture = pTexture;

    hResult = S_OK;
Exit0:
	SAFE_RELEASE(pTexture);
    return hResult;
}

HRESULT KG3DTextureTable::IELoadResourceFromFile(
    const CHAR cszFileName[], 
    DWORD uFileNameHash,
    DWORD uOption, 
    IKG3DResourceBase **ppRetResource
)
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    ASSERT(cszFileName);
    ASSERT(ppRetResource);
    hRetCode = LoadResourceFromFile(cszFileName, uFileNameHash, uOption, ppRetResource);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DTextureTable::SetTexture(DWORD Index, DWORD TextureID)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;

    LPDIRECT3DTEXTURE9 pD3DTexture = NULL; 
	KG3DTexture *pTexture = NULL;
	hResult = GetResource(&pTexture, TextureID);
	KG_COM_PROCESS_ERROR(hResult);
    KG_PROCESS_ERROR(pTexture);

    pD3DTexture = pTexture->GetTexture();
	hResult = g_pd3dDevice->SetTexture(Index, pD3DTexture);
	KG_COM_PROCESS_ERROR(hResult);

	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pTexture);
	return hRetCode;
}

HRESULT KG3DTextureTable::ReloadFromFile()
{
	AutoReload();
	return S_OK;
}

////////////////////////////////////////////////
//	resource manager
bool  KG3DTextureTable::Resources(DWORD TextureID)
{
	KG3DTexture *pTexture = NULL;
	bool bReturn          = false;
	HRESULT hResult       = E_FAIL;
	KG_PROCESS_SUCCESS(TextureID == STID_ERROR);

	hResult = GetResource(&pTexture, TextureID);
	KG_COM_PROCESS_ERROR(hResult);
Exit1:
	bReturn = true;
Exit0:
	SAFE_RELEASE(pTexture);
	return bReturn;
}

void  KG3DTextureTable::EvictResources( DWORD videoMax  )
{
    g_nNumTextureDynamicLoadedFrame = 0;
	return ;
	//if ( videoMax >= g_pd3dDevice->GetAvailableTextureMem( ) )
	//BOOL bCount = g_dwRenderCount % 10 <= 0;
	//if(!bCount)
	//	return;
	UINT uMem = g_pd3dDevice->GetAvailableTextureMem();

	int nReleaseTime = 500;
	if(uMem <= 40*102400 )
		nReleaseTime = 100;

	int nCount = 0;

	m_Lock.Lock();

	map<DWORD,KG3DTexture*>::iterator m = m_mapResource.begin();
	map<DWORD,KG3DTexture*>::iterator mend = m_mapResource.end();

	while ( m != mend )
	{
		KG3DTexture* pResource = m->second;
		//ASSERT( pResource );

		/*if(pResource->m_nReference <= 0)
		{
			if ((pResource->m_bCanReload )&& g_eEngineOption.bUseMultiThreadLoad &&
				(pResource->m_eDynamicLoadState == KG3DTexture::STATE_MULTITHREADREAD_FINISH))
			{
				listZeroTexture.push_back(pResource);
			}
		}
		else*/
		if(pResource)
		{
			if ( (int)(g_dwRenderCount - pResource->GetLastUseTime()) >= nReleaseTime )
			{
				if ( pResource->EvictResources() )
				{
					nCount++;
					if(nCount>= 5)
						break;
				}
			}
		}

		m++;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		nCount = 0;

		list<KG3DTexture*>::iterator i = m_lstReleaseQueue.begin();
		list<KG3DTexture*>::iterator iend = m_lstReleaseQueue.end();
		while (i!=iend)
		{
			KG3DTexture* pResource = *i;
			if(pResource)
			{
				if ( (int)(g_dwRenderCount - pResource->GetLastUseTime()) >= nReleaseTime )
				{
					if ( pResource->EvictResources() )
					{
						nCount++;
						if(nCount>= 5)
							break;
					}
				}
			}
			i++;
		}
	}

	m_Lock.Unlock();
	//////////////////////////////////////////////////////////////////////////
	
}


HRESULT KG3DTextureTable::Get1NewResourcePointer(KG3DTexture** ppT, DWORD dwType,DWORD_PTR Option)
{
	HRESULT hRetCode = E_FAIL;

	KG_PROCESS_ERROR(ppT);
	
	switch(dwType)
	{
	case TEXOP_COMMON:
		{
			(*ppT) = new KG3DTexture;
			break;
		}
	case TEXOP_ANIMATION:
		{
			(*ppT) = new KG3DTextureAnimation;
			break;
		}
    case TEXOP_FLASH :
        {
            (*ppT) = new KG3DFlashTexture;
            break;
        }
	default:
		{
			(*ppT) = new KG3DTexture;
			break;
		}
	}
	KG_PROCESS_ERROR(*ppT);
	(*ppT)->Init();
	(*ppT)->SetType(dwType);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DTextureTable::CreateTexture(unsigned int uWidth,
										unsigned int uHeight,
										D3DFORMAT format,
										TEXTYPE	texType,
										KG3DTexture** ppTexture,
										LPCTSTR	fileNameForHash /*= NULL*/)
{
	HRESULT hr = E_FAIL;
	KG3DTexture* pTextureCreated = NULL;
	KG_PROCESS_ERROR(NULL != ppTexture);

	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	DWORD_PTR Option = 0;
	if (NULL != fileNameForHash)
	{
		static KG3DGet1NewResourceOptionStruct TempStruct;
		TempStruct.dwOptionIdentifier = KG3DGET1NEWRESOURCE_FILE_NAME_FOR_HASH;
		TempStruct.czFileName = fileNameForHash;
		Option = reinterpret_cast<DWORD_PTR>(&TempStruct);
	}

	hr = Get1NewResource(&pTextureCreated, 0, Option);
	KG_COM_PROCESS_ERROR(hr);

	hr = pTextureCreated->CreateTexture(uWidth, uHeight, format, texType);
	KG_COM_PROCESS_ERROR(hr);
	
	*ppTexture = pTextureCreated;
	pTextureCreated = NULL;
	hr = S_OK;
Exit0:
	SAFE_RELEASE(pTextureCreated);
	return hr;
}

HRESULT KG3DTextureTable::AutoReload()
{
	//CTH 2008-01-22 2106 基于和KG3DTexture::LoadFromFile里面相同的原因，还是把这个操作限定在非客户端里面比较好
	m_Lock.Lock();

	if(! g_bClient && !g_eEngineOption.bUseLogical)
	{
		map<DWORD,KG3DTexture*>::iterator m = m_mapResource.begin();
		while(m!=m_mapResource.end())
		{
			KG3DTexture* pResource = m->second;
			if(pResource->IsFileChanged())
			{
				pResource->ForceReload();
				//pResource->Reload();
			}
			m++;
		}
	}

	g_cSceneTable.AutoReload();

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DTextureTable::UnInit()
{

	HRESULT hr = TKG3DResourceManagerBase<KG3DTexture>::UnInit();
	for (DWORD i = 0; i < STI_COUNT; i++)
	{
		SAFE_DELETE(m_SpecialTextures[i]);
	}

	UnInitDetailTables();

	SAFE_RELEASE(s_lpEnvMap);

	m_VideoMemTextureMgr.UnInit();

	return hr;
}

HRESULT KG3DTextureTable::Init()
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	hResult = TKG3DResourceManagerBase<KG3DTexture>::Init();
	KG_COM_PROCESS_ERROR(hResult);

	m_RefTableSize = sizeof(s_TextureTableTypes) / sizeof(TypeInfo);
	m_RefTable     = s_TextureTableTypes;
	m_pDefaultType = &s_TextureTableDefaultType;

	for (DWORD i = 0; i < STI_COUNT; i++)
	{
		if (s_strSpecialTextureFilePath[i][0] != '\0')
		{
			m_SpecialTextures[i] = new KG3DTexture;
			KG_PROCESS_ERROR(m_SpecialTextures[i]);

			m_SpecialTextures[i]->Init();

			m_SpecialTextures[i]->SetID(s_SpecialTextureID[i]);
			m_SpecialTextures[i]->AddRef();

			hResult = m_SpecialTextures[i]->LoadFromFile(s_strSpecialTextureFilePath[i], 
				0, 
				TEXOP_DISABLEDTX);
			KGLOG_COM_PROCESS_ERROR(hResult);
		}
	}

	if (NULL == s_lpEnvMap)
	{
		KG3DD3DXCreateCubeTextureFromFile(g_pd3dDevice, GetDefaultEnvMapName(), &s_lpEnvMap);
	}	

	m_VideoMemTextureMgr.Init();

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DTextureTable::GetResource(KG3DTexture** ppT, DWORD dwID)
{
	HRESULT hRetCode = E_FAIL;
	for (DWORD i = 1; i < STI_COUNT; i++)
	{
		if (dwID == static_cast<DWORD>(s_SpecialTextureID[i]))
		{
			*ppT = m_SpecialTextures[i];
			KG_PROCESS_ERROR(*ppT);

			(*ppT)->AddRef();
			goto Exit1;
		}
	}

	hRetCode = TKG3DResourceManagerBase<KG3DTexture>::GetResource(ppT, dwID);
	KG_COM_PROCESS_ERROR(hRetCode);
Exit1:
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}


static void CheckWidthAndHeight(unsigned int& uWidth, 
								unsigned int& uHeight
								)
{
	unsigned int uCeiling = 1;
	if (g_cGraphicsEngine.GetD3DCaps().TextureCaps & D3DPTEXTURECAPS_POW2)
	{
		while(uCeiling < uWidth)
		{
			uCeiling <<= 1;
		}
		uWidth = uCeiling;

		uCeiling = 1;
		while(uCeiling < uHeight)
		{
			uCeiling <<= 1;
		}
		uHeight = uCeiling;
	}
}

KG3DTexture* KG3DTextureTable::GetRenderTargetTexture(UINT uWidth, 
							   UINT uHeight, 
							   D3DFORMAT Format)
{
	KG3DTexture *pReturn = NULL;
	HRESULT hResult = E_FAIL;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	KGLOG_PROCESS_ERROR(uWidth);
	KGLOG_PROCESS_ERROR(uHeight);

	if (m_SpecialTextures[STI_RENDERTARGET] &&
		m_CurrentRenderTargetDesc.Width < uWidth ||
		m_CurrentRenderTargetDesc.Height < uHeight || 
		m_CurrentRenderTargetDesc.Format != Format)
	{
		SAFE_DELETE(m_SpecialTextures[STI_RENDERTARGET]);
	}

	if (!m_SpecialTextures[STI_RENDERTARGET])
	{
		CheckWidthAndHeight(uWidth, uHeight);
		hResult = g_cTextureTable.CreateTexture(uWidth, 
			uHeight, 
			Format, 
			TEXTYPE_RENDER_TARGET,
			&m_SpecialTextures[STI_RENDERTARGET]);
		KG_COM_PROCESS_ERROR(hResult);

		m_SpecialTextures[STI_RENDERTARGET]->SetID(static_cast<DWORD>(STID_RENDERTARGET));
		m_CurrentRenderTargetDesc.Height = uHeight;
		m_CurrentRenderTargetDesc.Width = uWidth;
		m_CurrentRenderTargetDesc.Format = Format;
	}

	pReturn = m_SpecialTextures[STI_RENDERTARGET];
Exit0:
	return pReturn;
}

HRESULT KG3DTextureTable::GetRenderTargetTexture(UINT Width, 
												 UINT Height, 
												 D3DFORMAT format, 
												 KG3DTexture** ppTexture, 
												 UINT uSizeIsFractionOfBackbufferSize/* = 0*/)
{
	HRESULT hr = E_FAIL;
	UINT	WidthUse = Width;
	UINT	HeightUse = Height;
	D3DFORMAT	FormatUse = format;
	UINT	NumMipLevelUse = 1;
	std::string	tsTemp = "data\\public\\defaultscene\\RT";
	KG3DTexture* pRetTex = NULL;
	tsTemp.reserve(MAX_PATH);
	TCHAR	tcsBuffer[MAX_PATH];
	KG_PROCESS_ERROR(g_pd3dDevice && ppTexture);
	
	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID == g_dwMainThreadID);

	KG_PROCESS_ERROR(uSizeIsFractionOfBackbufferSize >= 0 && uSizeIsFractionOfBackbufferSize <= 4);
	if(uSizeIsFractionOfBackbufferSize == 0)
	{
		KG_PROCESS_ERROR(Width > 0 && Width <= 4096 && Height > 0 && Height <= 4096);
		hr = D3DXCheckTextureRequirements(g_pd3dDevice, &WidthUse, &HeightUse, &NumMipLevelUse, D3DUSAGE_RENDERTARGET, &FormatUse, D3DPOOL_DEFAULT);
		KG_COM_PROCESS_ERROR(hr);
	}
	else//如果uSizeIsFractionOfBackbufferSize不为0，那么使用D3DX_DEFAULT的值来哈希，保证唯一
	{
		WidthUse = D3DX_DEFAULT/uSizeIsFractionOfBackbufferSize;
		HeightUse = D3DX_DEFAULT/uSizeIsFractionOfBackbufferSize;
	}
	//用Width，Height，Size创建唯一哈希
	{
		BOOL bRet = KG_CUSTOM_HELPERS::TValueToString(WidthUse, tcsBuffer);
		KG_PROCESS_ERROR(bRet);
		tsTemp += tcsBuffer;
		tsTemp += "_";
		bRet = KG_CUSTOM_HELPERS::TValueToString(HeightUse, tcsBuffer);
		KG_PROCESS_ERROR(bRet);
		tsTemp += tcsBuffer;
		tsTemp += "_";

		UINT uFormatTemp = FormatUse;
		bRet = KG_CUSTOM_HELPERS::TValueToString(uFormatTemp, tcsBuffer);
		KG_PROCESS_ERROR(bRet);
		tsTemp += tcsBuffer;

		tsTemp += ".tga";
	}
		

	{
		DWORD dwID = g_FileNameHash(tsTemp.c_str());
		
		hr = GetResource(&pRetTex, dwID);//因为如果请求了uSizeIsFractionOfBackbufferSize的话，哈希是用D3DX_DEFAULT计算的，是不变的，和BackBuffer真正大小无关
		if (SUCCEEDED(hr))
		{
			*ppTexture = pRetTex;//GetResource已经加过引用计数了
			return hr;
		}
		
		KG3DGet1NewResourceOptionStruct	Get1NewOpStruct;
		Get1NewOpStruct.dwOptionIdentifier = KG3DGET1NEWRESOURCE_FILE_NAME_FOR_HASH;
		Get1NewOpStruct.czFileName = tsTemp.c_str();

		hr = this->Get1NewResource(&pRetTex, 0, (DWORD_PTR)&Get1NewOpStruct);
		KG_COM_PROCESS_ERROR(hr);

		//如果请求了uSizeIsFractionOfBackbufferSize，WidthUse和HeightUse不会被使用
		hr = pRetTex->CreateTexture(WidthUse, HeightUse, FormatUse, TEXTYPE_RENDER_TARGET, uSizeIsFractionOfBackbufferSize);
		KG_COM_PROCESS_ERROR(hr);

#if defined(_DEBUG) | defined(DEBUG)//检查得到的大小和格式是否相符
		if (uSizeIsFractionOfBackbufferSize == 0)
		{
			D3DSURFACE_DESC SFDesc;
			hr = pRetTex->GetSurfaceDesc(0, &SFDesc);
			_ASSERTE(SUCCEEDED(hr));
			_ASSERTE(SFDesc.Width == WidthUse && SFDesc.Height == HeightUse && SFDesc.Format == FormatUse);
		}
#endif

		*ppTexture = pRetTex;
		pRetTex = NULL;
	}
	SAFE_RELEASE(pRetTex);
	return S_OK;
Exit0:
	SAFE_RELEASE(pRetTex);
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DTextureTable::Detail_GetBodyColor(int nTable,int nIndex,D3DCOLORVALUE& vBodyColor,D3DCOLORVALUE& vBodyColor1,float& fMax,float& fMin,float& fScale,float& fEnv,float& fSpecular,LPSTR pName,
											  D3DCOLORVALUE& vColor0,float& fColorScale0,float& fUVScale0,float& fDetail0_Env,float& fDetail0_Specular,LPSTR pFileName0,
											  D3DCOLORVALUE& vColor1,float& fColorScale1,float& fUVScale1,float& fDetail1_Env,float& fDetail1_Specular,LPSTR pFileName1,
											  D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture

)
{
	DetailTable& vTable = GetDetailTable(nTable);

	KGLOG_PROCESS_ERROR( nIndex>=0 && nIndex < (int)vTable.m_vecDetailColorDefine.size() );

	KG3DDetail_Define* pbodycolor = vTable.m_vecDetailColorDefine[nIndex];
	KG_PROCESS_ERROR(pbodycolor);

	vBodyColor = pbodycolor->vBodyColor;
	vBodyColor1 = pbodycolor->vBodyColor1;
	fMax   = pbodycolor->fMax;
	fMin   = pbodycolor->fMin;
	fScale = pbodycolor->fScale;
	wsprintf(pName,"%s",pbodycolor->szName.c_str());
	fEnv   = pbodycolor->fEnv;
	fSpecular = pbodycolor->fSpecular;

	vColor0      = pbodycolor->vColor0     ;
	fColorScale0 = pbodycolor->fColorScale0;
	fUVScale0    = pbodycolor->fUVScale0   ;
	wsprintf(pFileName0,"%s",pbodycolor->szFileName0.c_str());
	vColor1      = pbodycolor->vColor1     ;
	fColorScale1 = pbodycolor->fColorScale1;
	fUVScale1    = pbodycolor->fUVScale1   ;
	wsprintf(pFileName1,"%s",pbodycolor->szFileName1.c_str());

	fDetail0_Env      = pbodycolor->fEnv0;
	fDetail0_Specular = pbodycolor->fSpecular0;
	fDetail1_Env      = pbodycolor->fEnv1;
	fDetail1_Specular = pbodycolor->fSpecular1;

	(*pFlow_Color) = pbodycolor->Flow_Color;
	(*pfFlow_ColorScale) = pbodycolor->Flow_ColorScale;
	(*pfFlow_UVScale) = pbodycolor->Flow_UVScale;
	(*pvSpeed) = pbodycolor->Flow_Speed;
	wsprintf(pFlowTexture,"%s",pbodycolor->Flow_Texture.c_str());

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTextureTable::Detail_SetBodyColor(int nTable,int nIndex,D3DCOLORVALUE  vBodyColor,D3DCOLORVALUE  vBodyColor1,float  fMax,float  fMin,float  fScale,float fEnv,float fSpecular,LPSTR pName,
											  D3DCOLORVALUE  vColor0,float  fColorScale0,float  fUVScale0,float fDetail0_Env,float fDetail0_Specular,LPSTR pFileName0,
											  D3DCOLORVALUE  vColor1,float  fColorScale1,float  fUVScale1,float fDetail1_Env,float fDetail1_Specular,LPSTR pFileName1,
											  D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture)
{
	DetailTable& vTable = GetDetailTable(nTable);

	if(nIndex >= (int)vTable.m_vecDetailColorDefine.size())
	{
		vTable.m_vecDetailColorDefine.resize(nIndex+1,NULL);
	}

	KG3DDetail_Define* pbodycolor = vTable.m_vecDetailColorDefine[nIndex];
	KG_PROCESS_ERROR(pbodycolor);

	pbodycolor->vBodyColor = vBodyColor;
	pbodycolor->vBodyColor1 = vBodyColor1;
	pbodycolor->fMax   = fMax  ;
	pbodycolor->fMin   = fMin  ;
	pbodycolor->fScale = fScale;
	pbodycolor->szName = pName ;
	pbodycolor->fEnv   = fEnv  ;
	pbodycolor->fSpecular = fSpecular;

	pbodycolor->vColor0      = vColor0     ;
	pbodycolor->fColorScale0 = fColorScale0;
	pbodycolor->fUVScale0    = fUVScale0   ;
	pbodycolor->szFileName0  = pFileName0  ;
	pbodycolor->vColor1      = vColor1     ;
	pbodycolor->fColorScale1 = fColorScale1;
	pbodycolor->fUVScale1    = fUVScale1   ;
	pbodycolor->szFileName1  = pFileName1  ;

	pbodycolor->fEnv0        = fDetail0_Env;
	pbodycolor->fSpecular0   = fDetail0_Specular;
	pbodycolor->fEnv1        = fDetail1_Env;
	pbodycolor->fSpecular1   = fDetail1_Specular;

	pbodycolor->Flow_Color   = Flow_Color;
	pbodycolor->Flow_ColorScale   = fFlow_ColorScale;
	pbodycolor->Flow_UVScale  = fFlow_UVScale;
	pbodycolor->Flow_Speed  = vSpeed;
	pbodycolor->Flow_Texture  = pFlowTexture;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTextureTable::LoadDetailDefines()
{
	TCHAR szName[MAX_PATH] = {"Data\\public\\DetailDefine.ini"};

	IIniFile* pFile = g_OpenIniFile(szName);
	KGLOG_PROCESS_ERROR(pFile);

	int nTable = 0;
	pFile->GetInteger("MAIN","TableCount",0,&nTable);

	for (int i=0;i<nTable;i++)
	{
		TCHAR szTableName[MAX_PATH];
		TCHAR szSection  [MAX_PATH];
		wsprintf(szSection,"Table%d",i);

		int nBodyDefine   = 0;

		pFile->GetInteger(szSection,"BodyDefineCount",0,&nBodyDefine);
		pFile->GetString (szSection,"TableName","",szTableName,MAX_PATH);

		Detail_SetDefineTableName(i,szTableName);

		for (int n=0;n<nBodyDefine;n++)
		{
			wsprintf(szSection,"Table%dBodyDefine%d",i,n);
			BOOL bExist = FALSE;
			pFile->GetInteger(szSection,"bExist",FALSE,&bExist);

			if(bExist)
			{
				D3DCOLORVALUE vBodyColor;
				D3DCOLORVALUE vBodyColor1;
				float         fMax;
				float         fMin;
				float         fScale;
				float         fEnv;
				float         fSpecular;
				TCHAR         sName[MAX_PATH];

				pFile->GetMultiFloat(szSection,"vColor",(float*)&vBodyColor,4       );
				pFile->GetMultiFloat(szSection,"vBodyColor1",(float*)&vBodyColor1,4       );
				pFile->GetFloat     (szSection,"fMax"  ,1              ,&fMax   );
				pFile->GetFloat     (szSection,"fMin"  ,0              ,&fMin   );
				pFile->GetFloat     (szSection,"fScale",1              ,&fScale );
				pFile->GetString    (szSection,"sName" ,"",sName       ,MAX_PATH);
				pFile->GetFloat     (szSection,"fEnv"  ,1              ,&fEnv   );
				pFile->GetFloat     (szSection,"fSpecular",1           ,&fSpecular );

				D3DCOLORVALUE vColor0;
				TCHAR         szFileName0[MAX_PATH];
				float         fColorScale0;
				float         fUVScale0;
				float         fEnv0;
				float         fSpecular0;
				D3DCOLORVALUE vColor1;
				TCHAR         szFileName1[MAX_PATH];
				float         fColorScale1;
				float         fUVScale1;
				float         fEnv1;
				float         fSpecular1;

				D3DCOLORVALUE Flow_Color;
				float         Flow_ColorScale;//流光效果参数
				float         Flow_UVScale;
				D3DXVECTOR2   Flow_Speed;
				TCHAR         Flow_Texture[MAX_PATH];

				pFile->GetMultiFloat(szSection,"vColor0"     ,(float*)&vColor0,4           );
				pFile->GetString    (szSection,"szFileName0" ,"",szFileName0  ,MAX_PATH    );
				pFile->GetFloat     (szSection,"fColorScale0",1              ,&fColorScale0);
				pFile->GetFloat     (szSection,"fUVScale0"   ,0              ,&fUVScale0   );
				pFile->GetFloat     (szSection,"fEnv0"       ,0              ,&fEnv0       );
				pFile->GetFloat     (szSection,"fSpecular0"  ,0              ,&fSpecular0  );
				pFile->GetMultiFloat(szSection,"vColor1"     ,(float*)&vColor1,4           );
				pFile->GetString    (szSection,"szFileName1" ,"",szFileName1  ,MAX_PATH    );
				pFile->GetFloat     (szSection,"fColorScale1",1              ,&fColorScale1);
				pFile->GetFloat     (szSection,"fUVScale1"   ,0              ,&fUVScale1   );
				pFile->GetFloat     (szSection,"fEnv1"       ,0              ,&fEnv1       );
				pFile->GetFloat     (szSection,"fSpecular1"  ,0              ,&fSpecular1  );
 
				pFile->GetMultiFloat(szSection,"Flow_Color"  ,(float*)&Flow_Color,4        );
				pFile->GetFloat     (szSection,"Flow_ColorScale",0        ,&Flow_ColorScale);
				pFile->GetFloat     (szSection,"Flow_UVScale"   ,0        ,&Flow_UVScale   );
				pFile->GetMultiFloat(szSection,"Flow_Speed"  ,(float*)&Flow_Speed,2        );
				pFile->GetString    (szSection,"Flow_Texture","",Flow_Texture  ,MAX_PATH   );

				Detail_AddBodyDefine(i,n);

				Detail_SetBodyColor(i,n,vBodyColor,vBodyColor1,fMax,fMin,fScale,fEnv,fSpecular,sName,
					vColor0,fColorScale0,fUVScale0,fEnv0,fSpecular0,szFileName0,
					vColor1,fColorScale1,fUVScale1,fEnv1,fSpecular1,szFileName1,
					Flow_Color,Flow_ColorScale,Flow_UVScale,Flow_Speed,Flow_Texture);
			}
		}
	}
	pFile->Release();
	return S_OK;
Exit0:
	return E_FAIL;
} 

HRESULT KG3DTextureTable::SaveDetailDefines()
{
	TCHAR szName[MAX_PATH] = {"Data\\public\\DetailDefine.ini"};

    IIniFile* pFile = g_OpenIniFile(szName);
    if (pFile)
    {
        char szKeepFileName[MAX_PATH];
        wsprintf(szKeepFileName, "%s.bak", szName);
        CopyFile(szName, szKeepFileName, false);
    }
    SAFE_RELEASE(pFile);

    {
        FILE* pFileClear;
        pFileClear = fopen(szName, "w+");
        KG_PROCESS_ERROR(pFileClear);
        fclose(pFileClear);
        pFileClear = NULL;
    }

    pFile = g_OpenIniFile(szName,false,true);
    KGLOG_PROCESS_ERROR(pFile);

	int nTable = Detail_GetDefineTableCount();
	pFile->WriteInteger("MAIN","TableCount",nTable);

	for (int i=0;i<nTable;i++)
	{
		TCHAR szTableName[MAX_PATH];
		TCHAR szSection  [MAX_PATH];
		wsprintf(szSection,"Table%d",i);
		
		DetailTable& table = m_vecDetailTable[i];
		wsprintf(szTableName,"%s",table.szName.c_str());

		int nBodyDefine   = Detail_GetBodyDefineCount(i);

		pFile->WriteInteger(szSection,"BodyDefineCount",nBodyDefine);
		pFile->WriteString (szSection,"TableName",szTableName);

		for (int n=0;n<nBodyDefine;n++)
		{
			HRESULT hr = E_FAIL;

			wsprintf(szSection,"Table%dBodyDefine%d",i,n);

			D3DCOLORVALUE vBodyColor;
			D3DCOLORVALUE vBodyColor1;
			float         fMax;
			float         fMin;
			float         fScale;
			float         fEnv;
			float         fSpecular;
			TCHAR         sName[MAX_PATH];

			D3DCOLORVALUE vColor0;
			TCHAR         szFileName0[MAX_PATH];
			float         fColorScale0;
			float         fUVScale0;
			float         fEnv0;
			float         fSpecular0;
			D3DCOLORVALUE vColor1;
			TCHAR         szFileName1[MAX_PATH];
			float         fColorScale1;
			float         fUVScale1;
			float         fEnv1;
			float         fSpecular1;

			D3DCOLORVALUE Flow_Color;
			float         Flow_ColorScale;//流光效果参数
			float         Flow_UVScale;
			D3DXVECTOR2   Flow_Speed;
			TCHAR         Flow_Texture[MAX_PATH];

			BOOL bExist = FALSE;

			hr = Detail_GetBodyColor(i,n,vBodyColor,vBodyColor1,fMax,fMin,fScale,fEnv,fSpecular,sName,
				vColor0,fColorScale0,fUVScale0,fEnv0,fSpecular0,szFileName0,
				vColor1,fColorScale1,fUVScale1,fEnv1,fSpecular1,szFileName1,
				&Flow_Color,&Flow_ColorScale,&Flow_UVScale,&Flow_Speed,Flow_Texture
				);

			if(SUCCEEDED(hr))
			{
				bExist = TRUE;
			}
            else
                continue;

			pFile->WriteInteger(szSection,"bExist",bExist);

			pFile->WriteMultiFloat(szSection,"vColor",(float*)&vBodyColor,4 );
			pFile->WriteMultiFloat(szSection,"vBodyColor1",(float*)&vBodyColor1,4 );
			pFile->WriteFloat     (szSection,"fMax"  ,fMax   );
			pFile->WriteFloat     (szSection,"fMin"  ,fMin   );
			pFile->WriteFloat     (szSection,"fScale",fScale );
			pFile->WriteString    (szSection,"sName" ,sName  );
			pFile->WriteFloat     (szSection,"fEnv"  ,fEnv   );
			pFile->WriteFloat     (szSection,"fSpecular",fSpecular );

			pFile->WriteMultiFloat(szSection,"vColor0"     ,(float*)&vColor0,4);
			pFile->WriteString    (szSection,"szFileName0" ,szFileName0);
			pFile->WriteFloat     (szSection,"fColorScale0",fColorScale0);
			pFile->WriteFloat     (szSection,"fUVScale0"   ,fUVScale0   );
			pFile->WriteFloat     (szSection,"fEnv0"       ,fEnv0       );
			pFile->WriteFloat     (szSection,"fSpecular0"  ,fSpecular0  );
			pFile->WriteMultiFloat(szSection,"vColor1"     ,(float*)&vColor1,4);
			pFile->WriteString    (szSection,"szFileName1" ,szFileName1);
			pFile->WriteFloat     (szSection,"fColorScale1",fColorScale1);
			pFile->WriteFloat     (szSection,"fUVScale1"   ,fUVScale1   );
			pFile->WriteFloat     (szSection,"fEnv1"       ,fEnv1       );
			pFile->WriteFloat     (szSection,"fSpecular1"  ,fSpecular1  );

			pFile->WriteMultiFloat(szSection,"Flow_Color"  ,(float*)&Flow_Color,4);
			pFile->WriteFloat     (szSection,"Flow_ColorScale",Flow_ColorScale);
			pFile->WriteFloat     (szSection,"Flow_UVScale"   ,Flow_UVScale   );
			pFile->WriteMultiFloat(szSection,"Flow_Speed"  ,(float*)&Flow_Speed,2);
			pFile->WriteString    (szSection,"Flow_Texture",Flow_Texture);
		}
	}

	pFile->Save(szName);
	pFile->Release();
	return S_OK;
Exit0:
	return E_FAIL;
}


KG3DTextureTable::DetailTable& KG3DTextureTable::GetDetailTable(int nTable)
{
	if(nTable<0)
		nTable = 0;

	if(nTable >= (int)m_vecDetailTable.size())
	{
		m_vecDetailTable.resize(nTable+1);
	}

	return m_vecDetailTable[nTable];
}

int KG3DTextureTable::Detail_GetDefineTableCount()
{
	return (int)m_vecDetailTable.size();
}

HRESULT KG3DTextureTable::Detail_AddDefineTable()
{
	DetailTable newtable;
	m_vecDetailTable.push_back(newtable);
	return S_OK;
}

int KG3DTextureTable::Detail_GetBodyDefineCount(int nTable)
{
	DetailTable& table = GetDetailTable(nTable);
	return (int)table.m_vecDetailColorDefine.size();
}

HRESULT KG3DTextureTable::Detail_GetDefineTableName(int nTable,LPSTR pName)
{
	DetailTable& table = GetDetailTable(nTable);
	wsprintf(pName,"%s",table.szName.c_str());
	return S_OK;
}

HRESULT KG3DTextureTable::Detail_SetDefineTableName(int nTable,LPSTR pName)
{
	DetailTable& table = GetDetailTable(nTable);
	table.szName = pName;
	return S_OK;
}

HRESULT KG3DTextureTable::Detail_AddBodyDefine(int nTable,int nIndex)
{
	DetailTable& table = GetDetailTable(nTable);

	KGLOG_PROCESS_ERROR(nIndex>=0);
	if(nIndex >= (int)table.m_vecDetailColorDefine.size())
	{
		table.m_vecDetailColorDefine.resize(nIndex+1,NULL);
	}

	KG3DDetail_Define* pDefine = table.m_vecDetailColorDefine[nIndex];

	if(!pDefine)
	{
		pDefine = g_cDetailDefinePool.RequestResource();
		table.m_vecDetailColorDefine[nIndex] = pDefine;
	}
	KGLOG_PROCESS_ERROR(pDefine);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTextureTable::Detail_DeleteDefineTable(int nTable)
{
	int S = 0;
	vector<DetailTable>::iterator i = m_vecDetailTable.begin();
	vector<DetailTable>::iterator iend = m_vecDetailTable.end();
	while (i!=iend)
	{
		if(S == nTable)
		{
			m_vecDetailTable.erase(i);
			return S_OK;
		}
		i++;
		S++;
	}

	return E_FAIL;
}

HRESULT KG3DTextureTable::Detail_DeleteBodyDefine(int nTable,int nIndex)
{
	DetailTable& table = GetDetailTable(nTable);

	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)table.m_vecDetailColorDefine.size());
	
	KG3DDetail_Define* pDefine = table.m_vecDetailColorDefine[nIndex];
	if(pDefine)
	{
		table.m_vecDetailColorDefine[nIndex] = NULL;
		g_cDetailDefinePool.FreeResource(pDefine);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTextureTable::UnInitDetailTables()
{
	for (size_t i=0;i<m_vecDetailTable.size();i++)
	{
		DetailTable& vTable = m_vecDetailTable[i];

		for (size_t s=0;s<vTable.m_vecDetailColorDefine.size();s++)
		{
			if(vTable.m_vecDetailColorDefine[s])
			{
				g_cDetailDefinePool.FreeResource(vTable.m_vecDetailColorDefine[s]);
			}
		}
		vTable.m_vecDetailColorDefine.clear();
	}
	m_vecDetailTable.clear();
	return S_OK;
}

LPDIRECT3DCUBETEXTURE9 KG3DTextureTable::GetDefaultEnvMap()
{
	return s_lpEnvMap;
}

LPCTSTR KG3DTextureTable::GetDefaultEnvMapName()
{
	return _T("\\data\\public\\evn.dds");
}

HRESULT KG3DTextureTable::OnLostDevice()
{
	HRESULT hr = E_FAIL;
	std::map<USIZE, LPDIRECT3DTEXTURE9>::iterator i = m_mapChessboardTextures.begin();
	while (i != m_mapChessboardTextures.end())
	{
		SAFE_RELEASE(i->second);
		i++;
	}
	m_mapChessboardTextures.clear();


	hr = TKG3DResourceManagerBase<KG3DTexture>::OnLostDevice();
	m_VideoMemTextureMgr.UnInit();

	return hr;
}


HRESULT KG3DTextureTable::ReportResourceUse(LPSTR pTableName)
{
	DWORD dwSize = GetResourceCount();
	KGLogPrintf(KGLOG_ERR, "=======================================================");
	KGLogPrintf(KGLOG_ERR, "%d %s Resource",dwSize,pTableName);

	int N = 0;
	map<DWORD,KG3DTexture*>::iterator m = m_mapResource.begin();
	while(m != m_mapResource.end())
	{
		KG3DTexture* pResource = m->second;
		if(pResource)
			KGLogPrintf(KGLOG_ERR, "%d %d %d %s",N,
			pResource->m_Desc.Width,
			pResource->m_Desc.Height,
			pResource->m_scName.c_str());
		m++;
		N++;
	}

	TypeExclusiveList::iterator itExclusive = m_listExclusiveRes.begin();
	while (itExclusive != m_listExclusiveRes.end())
	{
		KG3DTexture *pResource = *itExclusive;
		if(pResource)
			if(pResource)
				KGLogPrintf(KGLOG_ERR, "%d %d %d %s",N,
				pResource->m_Desc.Width,
				pResource->m_Desc.Height,
				pResource->m_scName.c_str());
		itExclusive++;
		N++;
	}
	return S_OK;
}

KG3DVideoMemTextureTable::KG3DVideoMemTextureTable()
{
	;
}

KG3DVideoMemTextureTable::~KG3DVideoMemTextureTable()
{
	;
}

HRESULT KG3DVideoMemTextureTable::Init()
{
	return S_OK;
}

HRESULT KG3DVideoMemTextureTable::UnInit()
{
	map<KG3DTEXTURE_DESC,VIDEOMEMPOOL*>::iterator i = m_mapTextures.begin();
	map<KG3DTEXTURE_DESC,VIDEOMEMPOOL*>::iterator iend = m_mapTextures.end();
	while (i!=iend)
	{
		VIDEOMEMPOOL* pPool = i->second;
		SAFE_DELETE(pPool);
		i++;
	}
	m_mapTextures.clear();

	return S_OK;
}

KG3DVideoMemTexture* KG3DVideoMemTextureTable::RequestResource(KG3DTexture* pUser)
{
	HRESULT hr = E_FAIL;

	KG3DVideoMemTexture* pOut = NULL;
	VIDEOMEMPOOL* pPool = NULL;
	map<KG3DTEXTURE_DESC,VIDEOMEMPOOL*>::iterator iFind;

	KGLOG_PROCESS_ERROR(pUser);

	iFind = m_mapTextures.find(pUser->m_Desc);

	if(iFind == m_mapTextures.end())
	{
		pPool = new VIDEOMEMPOOL;
		KGLOG_PROCESS_ERROR(pPool);
		m_mapTextures[pUser->m_Desc] = pPool;
	}
	else
	{
		pPool = iFind->second;
	}

	hr = pPool->GetOneShareResource(&pOut,pUser);
	KG_COM_CHECK_ERROR(hr);
Exit0:
	return pOut;
}

HRESULT  KG3DVideoMemTextureTable::FreeResource(KG3DTexture* pUser,KG3DVideoMemTexture* pResource)
{
	HRESULT hr = E_FAIL;

	KG3DVideoMemTexture* pOut = NULL;
	VIDEOMEMPOOL* pPool = NULL;
	map<KG3DTEXTURE_DESC,VIDEOMEMPOOL*>::iterator iFind;

	KGLOG_PROCESS_ERROR(pUser);

	iFind = m_mapTextures.find(pUser->m_Desc);
	KGLOG_PROCESS_ERROR(iFind != m_mapTextures.end());

	pPool = iFind->second;

	hr = pPool->FreeResource(pUser,pResource);
	KG_COM_CHECK_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}
