//////////////////////////////////////////////////////////////////////////
// File : KG3DSelectTexture.cpp
// Creator : FengBo
// Date : 2007-10-10
// Desc : KG3DSelectTexture.hµÄÊµÏÖ
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSFXSelectTexture.h"
#include "KG3DTextureTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFX.h"
#include "KG3DSceneSceneEditor.h"

#include "KG3DModel.h"
#include "KG3DRepresentObject.h"

#include "KG3DSFXProjectionTextureMgr.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL g_bOpenSelectTexture = TRUE;

KG3DSFXSelectTexture::KG3DSFXSelectTexture()
: m_pTexture(NULL)
, m_nTextureWidth(0)
, m_nTextureHeight(0)
, m_vec3PosCenter(0, 0, 0)
{
	m_pSFX =NULL;
	m_fAlpha = 1;
	m_enFoot = DEFAULT_FOOT;
}

KG3DSFXSelectTexture::KG3DSFXSelectTexture(KG3DSFX* pSFX)
: m_pTexture(NULL)
, m_nTextureWidth(0)
, m_nTextureHeight(0)
, m_vec3PosCenter(0, 0, 0)
{
	m_pSFX = pSFX;
	m_fAlpha = 1;
	m_enFoot = DEFAULT_FOOT;
}

KG3DSFXSelectTexture::~KG3DSFXSelectTexture()
{
    SAFE_RELEASE(m_pTexture);
}

IEKG3DTexture* KG3DSFXSelectTexture::GetTexture()
{
	return m_pTexture;
}

void KG3DSFXSelectTexture::Update()
{
	m_fAlpha -= 0.05f;
}

HRESULT KG3DSFXSelectTexture::LoadTexture(const char *szTextureFileName)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DTexture* pTexture = NULL;
	UINT width = 0;
    UINT height = 0;

	hRetCode = g_cTextureTable.LoadResourceFromFile(szTextureFileName, 0, TEXOP_DISABLEDTX, (IKG3DResourceBase**)&pTexture);
	KG_COM_PROCESS_ERROR(hRetCode);

    SAFE_RELEASE(m_pTexture);

	m_pTexture = pTexture;

    m_pTexture->GetSize(&width, &height);
	m_nTextureWidth = width;
	m_nTextureHeight = height;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSFXSelectTexture::SetTexture(IEKG3DTexture *pTexture)
{
	UINT width = 0;
    UINT height = 0;

    KG_PROCESS_ERROR(pTexture);

	SAFE_RELEASE(m_pTexture);

	m_pTexture = dynamic_cast<KG3DTexture*>(pTexture);
    m_pTexture->AddRef();

	m_pTexture->GetSize(&width, &height);
	m_nTextureWidth = width;
	m_nTextureHeight = height;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSFXSelectTexture::SetTexturePos(D3DXVECTOR3 *pNewPos)
{
	KG_PROCESS_ERROR(pNewPos);
	m_vec3PosCenter = *pNewPos;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSFXSelectTexture::GetTexturePos(D3DXVECTOR3* pPos)
{
	KG_PROCESS_ERROR(pPos);

	(*pPos) = m_vec3PosCenter;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSFXSelectTexture::GetRect(D3DXVECTOR2& vLeftBottom,D3DXVECTOR2& vRightTop)
{
	vLeftBottom.x = m_vec3PosCenter.x - m_nTextureWidth * 0.5f;
	vLeftBottom.y = m_vec3PosCenter.z - m_nTextureHeight * 0.5f;
	vRightTop.x = vLeftBottom.x + m_nTextureWidth;
	vRightTop.y = vLeftBottom.y + m_nTextureHeight;

	return S_OK;
}

DWORD KG3DSFXSelectTexture::GetTextureWidth()
{
	return m_nTextureWidth;
}

void KG3DSFXSelectTexture::SetTextureWidth(DWORD dwWidth)
{
	m_nTextureWidth = dwWidth;
}

DWORD KG3DSFXSelectTexture::GetTextureHeight()
{
	return m_nTextureHeight;
}

void KG3DSFXSelectTexture::SetTextureHeight(DWORD dwHeight)
{
	m_nTextureHeight = dwHeight;
}

float KG3DSFXSelectTexture::GetAlpha()
{
	return m_fAlpha;
}

void KG3DSFXSelectTexture::SetAlpha(float fAlpha)
{
	m_fAlpha = fAlpha;
}

HRESULT KG3DSFXSelectTexture::SetSFX(KG3DSFX* pSFX)
{
	HRESULT hr = E_FAIL;

	KG_PROCESS_ERROR(pSFX);

	m_pSFX = pSFX;

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSFXSelectTexture::ShutDown()
{
	return S_OK;
}

KG3DSFXSelectMark::KG3DSFXSelectMark(KG3DSFX* pSFX)
{
	m_pSFX = pSFX;
	m_pTexture = NULL;
	m_lpIB = NULL;
	m_lpVB = NULL;
	m_nTextureWidth = 64;
	m_nTextureHeight = 64;
	m_dwTerraType = 0;
}

KG3DSFXSelectMark::~KG3DSFXSelectMark()
{
	SAFE_RELEASE(m_pTexture);
}

HRESULT KG3DSFXSelectMark::LoadTexture(const char *szTextureFileName)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	KG3DTexture* pTexture = NULL;
    UINT width = 0;
    UINT height = 0;

	hRetCode = g_cTextureTable.LoadResourceFromFile(szTextureFileName, 0, 0, (IKG3DResourceBase**)&pTexture);
	KG_COM_PROCESS_ERROR(hRetCode);

	SAFE_RELEASE(m_pTexture);

	m_pTexture = pTexture;

	m_pTexture->GetSize(&width, &height);
	m_nTextureWidth = width;
	m_nTextureHeight = height;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSFXSelectMark::GetCurPosWidthHeight(D3DXVECTOR3* pPos,DWORD* pWidth,DWORD* pHeight)
{
	if(!m_pSFX)
		return E_FAIL;
	D3DXMATRIX matSFXWorld;
	D3DXQUATERNION quaRot;
	D3DXVECTOR3 vScale,vTran;
	matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();

	D3DXMatrixDecompose(&vScale,&quaRot,&vTran,&matSFXWorld);
	if (pPos)
		*pPos = vTran;
	if (pWidth)
		*pWidth = static_cast<DWORD>(m_nTextureWidth * vScale.x);
	if(pHeight)
		*pHeight = static_cast<DWORD>(m_nTextureHeight* vScale.z);
	return S_OK;
}

D3DXVECTOR3 KG3DSFXSelectMark::GetCurPos()
{
	if(!m_pSFX)
		return D3DXVECTOR3(0,0,0);
	D3DXMATRIX matSFXWorld;
	D3DXQUATERNION quaRot;
	D3DXVECTOR3 vScale,vTran;
	matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
	
	D3DXMatrixDecompose(&vScale,&quaRot,&vTran,&matSFXWorld);

	return vTran;
}

DWORD KG3DSFXSelectMark::GetWidth()
{
	D3DXMATRIX matSFXWorld;
	D3DXQUATERNION quaRot;
	D3DXVECTOR3 vScale,vTran;
	KG_PROCESS_ERROR(m_pSFX);
	matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
	D3DXMatrixDecompose(&vScale,&quaRot,&vTran,&matSFXWorld);
	DWORD dwWidth = static_cast<DWORD>(m_nTextureWidth * vScale.x);
	return dwWidth;
Exit0:
	return m_nTextureWidth;
}

DWORD KG3DSFXSelectMark::GetHeight()
{
	D3DXMATRIX matSFXWorld;
	D3DXQUATERNION quaRot;
	D3DXVECTOR3 vScale,vTran;
	KG_PROCESS_ERROR(m_pSFX);
	matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
	D3DXMatrixDecompose(&vScale,&quaRot,&vTran,&matSFXWorld);
	DWORD dwHeight = static_cast<DWORD>(m_nTextureHeight * vScale.z);
	return dwHeight;
Exit0:
	return m_nTextureHeight;
}

DWORD KG3DSFXSelectMark::GetTextureWidth()
{
	return m_nTextureWidth;
}

void KG3DSFXSelectMark::SetTextureWidth(DWORD dwWidth)
{
	m_nTextureWidth = dwWidth;
}

DWORD KG3DSFXSelectMark::GetTextureHeight()
{
	return m_nTextureHeight;
}
void KG3DSFXSelectMark::SetTextureHeight(DWORD dwHeight)
{
	m_nTextureHeight = dwHeight;
}

IEKG3DTexture* KG3DSFXSelectMark::GetTexture()
{
	return m_pTexture;
}

HRESULT KG3DSFXSelectMark::CreatVertexBuffer()
{
	HRESULT hr = E_FAIL;
	VFormat::_Faces_Diffuse_Texture1 *pData = NULL;
	WORD *pIndex = NULL;

	if (!m_lpIB)
	{
		hr = g_pd3dDevice->CreateIndexBuffer(
			sizeof(WORD) * 6,
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_lpIB, NULL);
		KG_COM_PROCESS_ERROR(hr);

		hr = m_lpIB->Lock(0, 0, (VOID**)&pIndex, 0);
		KG_COM_PROCESS_ERROR(hr);

		pIndex[0] = 0 ; pIndex[1] = 1 ; pIndex[2] = 3;
		pIndex[3] = 0 ; pIndex[4] = 3 ; pIndex[5] = 2;

		hr = m_lpIB->Unlock();
		KG_COM_PROCESS_ERROR(hr);
	}

	if (!m_lpVB)
	{
		DWORD dwThreadID = GetCurrentThreadId();
		ASSERT(dwThreadID == g_dwMainThreadID);

		hr = g_pd3dDevice->CreateVertexBuffer(
			sizeof(VFormat::_Faces_Diffuse_Texture1) * 4,
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
			D3DFVF_Faces_Diffuse_Texture1, D3DPOOL_DEFAULT, &m_lpVB, NULL);
		KG_COM_PROCESS_ERROR(hr);
	}

	hr = m_lpVB->Lock(0, 0, (VOID**)&pData, D3DLOCK_DISCARD);
	KG_COM_PROCESS_ERROR(hr);

	FillVertexBuffer(pData);

	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);


	hr = S_OK;

Exit0:
	return hr;
}

void KG3DSFXSelectMark::FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData)
{
	int nResult  = false;

	D3DXMATRIX	matSFXWorld;
	D3DXVECTOR3	vPosition;
	D3DXVECTOR3	vPosVertex;
	int	  nStep      = 0;
	float fLeft      = 0.0f;
	float fTop		 = 0.0f;
	KG_PROCESS_ERROR(m_pSFX);
	matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();

	m_matSaveSFXWorld = matSFXWorld;

	vPosition   = D3DXVECTOR3(matSFXWorld._41,matSFXWorld._42,matSFXWorld._43);

	fLeft   = vPosition.x - m_nTextureWidth * 0.5f;
	fTop    = vPosition.z + m_nTextureHeight * 0.5f;
	
	for (int y = 0; y < 2; y++)
	{
		for (int x = 0; x < 2; x++)
		{
			vPosVertex.x = fLeft + x * m_nTextureWidth;
			vPosVertex.z = fTop  - y * m_nTextureHeight;
			vPosVertex.y = 1.0f;

			pData[nStep].diffuse = 0xffffffff;
			pData[nStep].p = vPosVertex;
			pData[nStep].tu1 =  (float)x ;
			pData[nStep].tv1 =  (float)(1 - y) ;
			nStep++;
		}
	}

	//Exit1:
	nResult = true;
Exit0:
	return;
}

HRESULT KG3DSFXSelectMark::RenderTexture()
{
	HRESULT hr = E_FAIL;

	DWORD dwADDRESSU;
	DWORD dwADDRESSV;

	CreatVertexBuffer();

	KG3DRenderState RenderState;
	//GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	RenderState.SetRenderState(D3DRS_ALPHAREF,0x01);
	RenderState.SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	hr = g_pd3dDevice->SetStreamSource(0, m_lpVB, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
	KG_COM_PROCESS_ERROR(hr);
	
	hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	KG_COM_PROCESS_ERROR(hr);

	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &dwADDRESSU);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSV, &dwADDRESSV);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	
	hr = g_pd3dDevice->SetIndices(m_lpIB);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
	KG_COM_PROCESS_ERROR(hr);
	
	hr = g_pd3dDevice->SetTexture(0, m_pTexture ? m_pTexture->GetTexture() : NULL);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,4, 0, 2);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, dwADDRESSU);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, dwADDRESSV);
	KG_COM_PROCESS_ERROR(hr);
	
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSFXSelectMark::OnLostDevice()
{
	SAFE_RELEASE(m_lpIB);
	SAFE_RELEASE(m_lpVB);

	return S_OK;
}

HRESULT KG3DSFXSelectMark::OnResetDevice()
{
	return S_OK;
}