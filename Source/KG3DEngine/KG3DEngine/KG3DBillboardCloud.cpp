////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBillboardCloud.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-6-26 9:43:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DBillboardCloud.h"
#include "KG3DMaterial.h"
#include "KG3DTextureTable.h"
#include "KG3DGraphicsStructEx.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSystemEventListener.h"
#include "KG3DShaderHolder.h"
#include "KG3DShaderManager.h"
////////////////////////////////////////////////////////////////////////////////

extern BOOL g_bClient;

D3DXHANDLE		KG3DBillboardCloud::m_hEffectParams[em_count];

KG3DBillboardCloud::KG3DBillboardCloud(KG3DMaterial& Material)
:m_pVertexBuffer(NULL)
,m_pTextureColor(NULL)
,m_pTextureNormal(NULL)
,m_dwPlaneCount(0)
,m_Material(Material)
,m_ulRefCount(1)
{

}

KG3DBillboardCloud::~KG3DBillboardCloud()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pTextureColor);
	SAFE_RELEASE(m_pTextureNormal);
}

KG3DBillboardCloud* KG3DBillboardCloud::Build( LPCTSTR lpFileName, KG3DMaterial& Material, DWORD_PTR lParam )
{
	KG3DBillboardCloud* pRet = NULL;
	if(NULL != lpFileName)
	{
		pRet = BuildFromFile(lpFileName, Material, lParam);
	}
	else
	{
		KG3DBillboardCloudBuildStruct* pBuildStruct = reinterpret_cast<KG3DBillboardCloudBuildStruct*>(lParam);
		KG_PROCESS_ERROR(NULL != pBuildStruct 
							&& NULL != pBuildStruct->m_pBuffer
							&& NULL != pBuildStruct->m_pTexture);
		pRet = new KG3DBillboardCloud(Material);
		KG_PROCESS_ERROR(NULL != pRet);

		_ASSERTE(NULL == pRet->m_pVertexBuffer && NULL == pRet->m_pTextureColor);
		pRet->m_pVertexBuffer = pBuildStruct->m_pBuffer;
		pBuildStruct->m_pBuffer->AddRef();
		pRet->m_pTextureColor = pBuildStruct->m_pTexture;
		pBuildStruct->m_pTexture->AddRef();

		D3DVERTEXBUFFER_DESC	Desc;
		HRESULT hr = pRet->m_pVertexBuffer->GetDesc(&Desc);
		KG_COM_PROCESS_ERROR(hr);

		size_t iVertexSize = GetVertexStrideFromFVF(Desc.FVF);
		_ASSERTE(0 != iVertexSize);
		pRet->m_dwPlaneCount = Desc.Size / iVertexSize / 6;	//用三角型绘制，每6个点一个Plane
	}

	return pRet;
Exit0:
	SAFE_RELEASE(pRet);
	return NULL;	
}

KG3DBillboardCloud* KG3DBillboardCloud::BuildFromFile( LPCTSTR lpFileName, KG3DMaterial& Material, DWORD_PTR lParam )
{
	IFile* pFile = NULL;
	KG3DBillboardCloud* pRet = NULL;
	KG_PROCESS_ERROR(NULL != lpFileName);

	pRet = new KG3DBillboardCloud(Material);
	KG_PROCESS_ERROR(NULL != pRet);

	_ASSERTE(NULL == pRet->m_pTextureColor && NULL == pRet->m_pVertexBuffer && pRet->m_dwPlaneCount == 0);

	{
		VFormat::_Faces_Normal_Texture1* pVertexs = NULL;

		{
			TCHAR tczBillboardName[MAX_PATH];
			BOOL bRet = GetBillboardPath(lpFileName, tczBillboardName, NULL,NULL, _countof(tczBillboardName));
			KG_PROCESS_ERROR(bRet);

			pFile = g_OpenFile(tczBillboardName);
		}
		KG_PROCESS_ERROR(NULL != pFile);

		pFile->Read(&pRet->m_dwPlaneCount, sizeof(DWORD));

		UINT VertexBufferSize = pRet->m_dwPlaneCount * 6 * sizeof(VFormat::_Faces_Normal_Texture1);

		HRESULT hr = g_pd3dDevice->CreateVertexBuffer(
			VertexBufferSize,
			D3DUSAGE_WRITEONLY, 
			D3DFVF_Faces_Normal_Texture1,
			D3DPOOL_DEFAULT,
			&pRet->m_pVertexBuffer,
			NULL);
		KG_COM_PROCESS_ERROR(hr);

		hr = pRet->m_pVertexBuffer->Lock(0, 0, (void**)&pVertexs, 0);
		KG_COM_PROCESS_ERROR(hr);

		pFile->Read(pVertexs, VertexBufferSize);

		pRet->m_pVertexBuffer->Unlock();
		KG_COM_RELEASE(pFile);
	}
	{
		TCHAR tczTextureCName[MAX_PATH];
		TCHAR tczTextureNName[MAX_PATH];

		SAFE_RELEASE(pRet->m_pTextureColor);
		SAFE_RELEASE(pRet->m_pTextureNormal);

		BOOL bRet = GetBillboardPath(lpFileName, NULL, tczTextureCName,tczTextureNName, _countof(tczTextureCName));
		KG_PROCESS_ERROR(bRet);
		HRESULT hr = g_cTextureTable.LoadResourceFromFile(tczTextureCName
			, 0, /*TEXOP_DISABLEDTX*/TEXOP_RELOAD, (IKG3DResourceBase**)&pRet->m_pTextureColor);
		KG_COM_PROCESS_ERROR(hr);

		hr = g_cTextureTable.LoadResourceFromFile(tczTextureNName
			, 0, /*TEXOP_DISABLEDTX*/TEXOP_RELOAD, (IKG3DResourceBase**)&pRet->m_pTextureNormal);
		KG_COM_PROCESS_ERROR(hr);

		if (!g_bClient)
		{
			pRet->m_pTextureColor->ForceReload();
			pRet->m_pTextureNormal->ForceReload();
		}
	}

	return pRet;
Exit0:
	if (NULL != pRet)
	{
		SAFE_RELEASE(pRet->m_pVertexBuffer);
		SAFE_RELEASE(pRet->m_pTextureColor);
		SAFE_RELEASE(pRet->m_pTextureNormal);
		pRet->m_dwPlaneCount = 0;
	}

	SAFE_DELETE(pRet);
	KG_COM_RELEASE(pFile);
	
	return NULL;
}

BOOL KG3DBillboardCloud::GetBillboardPath( LPCTSTR lpFileName, LPTSTR lpBillboadPath, LPTSTR lpTextureColorPath,LPTSTR lpTextureNormalPath, size_t PathBufferSize )
{
	if(NULL != lpBillboadPath)
	{
		lpBillboadPath[0] = '\0';
		bool bRet = KG_CUSTOM_HELPERS::StrReplaceExtension(lpFileName, _T("bbc"), lpBillboadPath, PathBufferSize);
		KG_PROCESS_ERROR(bRet);
	}

	if (NULL != lpTextureColorPath)
	{
		lpTextureColorPath[0] = '\0';
		bool bRet = KG_CUSTOM_HELPERS::StrReplaceExtension(lpFileName, _T("C.png"), lpTextureColorPath, PathBufferSize);
		KG_PROCESS_ERROR(bRet);
	}

	if (NULL != lpTextureNormalPath)
	{
		lpTextureNormalPath[0] = '\0';
		bool bRet = KG_CUSTOM_HELPERS::StrReplaceExtension(lpFileName, _T("N.png"), lpTextureNormalPath, PathBufferSize);
		KG_PROCESS_ERROR(bRet);
	}

	return TRUE;
Exit0:
	return FALSE;
}

BOOL KG3DBillboardCloud::CheckIsBillboardExist( LPCTSTR lpFileName )
{
	TCHAR tczBillboardPath[MAX_PATH];
	TCHAR tczTextureCPath[MAX_PATH];
	TCHAR tczTextureNPath[MAX_PATH];

	BOOL bRet = GetBillboardPath(lpFileName, tczBillboardPath, tczTextureCPath,tczTextureNPath, MAX_PATH);
	if (bRet)
	{
		return g_IsFileExist(tczBillboardPath) && g_IsFileExist(tczTextureCPath) && g_IsFileExist(tczTextureNPath);
	}
	return FALSE;
}

HRESULT KG3DBillboardCloud::RenderBegin()
{
	UINT uPass = 0;
	g_pd3dDevice->SetFVF(D3DFVF_Faces_Normal_Texture1);

	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	KGLOG_PROCESS_ERROR(pShader);

	pShader->SetTechnique("tec0");
	pShader->Begin(&uPass,0);
	pShader->BeginPass(0);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloud::RenderEnd()
{
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	KGLOG_PROCESS_ERROR(pShader);

	pShader->EndPass();
	pShader->End();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloud::Render()
{	
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	KGLOG_PROCESS_ERROR(pShader);

	KG_PROCESS_ERROR( NULL != m_pVertexBuffer && NULL != m_pTextureColor&& NULL != m_pTextureNormal);

	if(m_dwPlaneCount != 0)
	{
		g_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VFormat::_Faces_Normal_Texture1));	//Mesh是共用的，Model不是共用的，注意这个

		g_pd3dDevice->SetTexture(0, m_pTextureColor ->GetTexture());
		g_pd3dDevice->SetTexture(1, m_pTextureNormal->GetTexture());
		g_pd3dDevice->SetTexture(2,NULL);

		GraphicsStruct::SaveMatrix rsMW(D3DTS_WORLD);

		D3DXMATRIX matView,matProj,mat;
		g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);

		mat = m_matWorld * matView * matProj;
		pShader->SetMatrix("WorldViewProj",&mat);

		pShader->CommitChanges();

		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwPlaneCount * 2);		
	}

Exit0:
	return S_OK;
}

ULONG STDMETHODCALLTYPE KG3DBillboardCloud::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DBillboardCloud::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT KG3DBillboardCloud::RenderBegin2()
{
	LPD3DXEFFECT p = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	_ASSERTE(NULL != p);
	{
		D3DXMATRIX matView, matProj;
		g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
		matView *= matProj;
		p->SetMatrix(m_hEffectParams[em_mat_vp], &matView);

		g_pd3dDevice->SetFVF(CUR_FVF);

		HRESULT hr = p->Begin(NULL, 0);
		if (FAILED(hr))
		{
			return E_FAIL;			
		}

		p->BeginPass(0);
	}
	return S_OK;
}

HRESULT KG3DBillboardCloud::RenderEnd2()
{
	LPD3DXEFFECT p = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	_ASSERTE(NULL != p);
	p->EndPass();
	p->End();
	return S_OK;
}

HRESULT KG3DBillboardCloud::Render2()
{
	if(NULL != m_pVertexBuffer)
		g_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(TypeVertexStruct));

	if(NULL != m_pTextureColor)
		g_pd3dDevice->SetTexture(0, m_pTextureColor->GetTexture());
	if(NULL != m_pTextureNormal)
		g_pd3dDevice->SetTexture(1, m_pTextureNormal->GetTexture());

	LPD3DXEFFECT p = g_cGraphicsTool.GetDefaultShader(DEFST_BILLBOARDCLOUD);
	_ASSERTE(NULL != p);
	if(m_dwPlaneCount > 0)
	{
		p->SetMatrix(m_hEffectParams[em_mat_world], &m_matWorld);
		p->CommitChanges();
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_dwPlaneCount * 2);	
	}
	return S_OK;
}
