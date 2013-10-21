#include "StdAfx.h"
#include "KG3DSelector.h"
#include "kg3dmodel.h"
#include "kg3dGraphicsTool.h"
#include "KG3DScene.h"
#include "IEEditor.h"
#include "KG3DGraphiceEngine.h"

extern TCHAR g_szConfigFilePath[MAX_PATH];

int MouseX = 0;
int MouseY = 0;

D3DXVECTOR3 vCenter;

float fFPS = 0.0f;
DWORD dwTime = 0;
DWORD dwFrame = 0;
int nFrameSkip = 3;
float fUpdateTime = 0.0f;

KG3DSelector g_Selector;
KG3DSelector::KG3DSelector() :
m_pRT(NULL),
m_pStencilRT(NULL),
m_pResult(NULL),
m_nHeight(0),
m_nWidth(0),
m_nScale(1),
m_dwIndex(0),
m_bSelected(FALSE),
m_pSelectedModel(NULL)
{

}

struct SelectorVertex 
{
	D3DXVECTOR4 vec4Pos;
	D3DXVECTOR2 vec2UV;
	static const DWORD dwFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
};

SelectorVertex ScreenRect[4];

KG3DSelector::~KG3DSelector()
{

}

HRESULT KG3DSelector::Init()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;

	IIniFile *pConfig = g_OpenIniFile(g_szConfigFilePath);
	int nResult = 0;
	KG_PROCESS_ERROR(pConfig);

	nResult = pConfig->GetInteger("KG3DENGINE", "SelectorScale", 2, &m_nScale);
	
	//处理Scale == 0的情况
	if (m_nScale == 0)
	{
		m_nScale = 1;
	}

	m_nWidth = static_cast<int>(g_cGraphicsEngine.GetPresentParameters().BackBufferWidth);
	m_nHeight = static_cast<int>(g_cGraphicsEngine.GetPresentParameters().BackBufferHeight);
	
	hResult= D3DXCreateTexture(g_pd3dDevice, 
		m_nWidth / m_nScale, 
		m_nHeight / m_nScale, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8, 
		D3DPOOL_DEFAULT, 
		&m_pStencilRT);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->CreateDepthStencilSurface(m_nWidth / m_nScale, 
		m_nHeight / m_nScale, 
		D3DFMT_D24X8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		FALSE, 
		&m_pStencilDepth, 
		NULL);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->CreateRenderTarget(1, 
		1, 
		D3DFMT_A8R8G8B8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		FALSE, 
		&m_pRT, 
		NULL);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = D3DXCreateTexture(g_pd3dDevice, 
		1, 
		1, 
		0, 
		0, 
		D3DFMT_A8R8G8B8, 
		D3DPOOL_SYSTEMMEM, 
		&m_pResult);
	KG_COM_PROCESS_ERROR(hResult);

	ScreenRect[0].vec4Pos = D3DXVECTOR4(0.0f, 0.0f, .0f, 1.0f);
	ScreenRect[1].vec4Pos = D3DXVECTOR4(static_cast<float>(m_nWidth), 0.0f, .0f, 1.0f);
	ScreenRect[2].vec4Pos = D3DXVECTOR4(static_cast<float>(m_nWidth), static_cast<float>(m_nHeight), .0f, 1.0f);
	ScreenRect[3].vec4Pos = D3DXVECTOR4(0.0f, static_cast<float>(m_nHeight), .0f, 1.0f);

	ScreenRect[0].vec2UV = D3DXVECTOR2(0.0f, 0.0f);
	ScreenRect[1].vec2UV = D3DXVECTOR2(1.0f, 0.0f);
	ScreenRect[2].vec2UV = D3DXVECTOR2(1.0f, 1.0f);
	ScreenRect[3].vec2UV = D3DXVECTOR2(0.0f, 1.0f);

	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pConfig);
	return hRetCode;
}

void KG3DSelector::UnInit()
{
	IIniFile *pConfig = g_OpenIniFile(g_szConfigFilePath, false, true);
	int nResult = 0;
	KG_PROCESS_ERROR(pConfig);

	nResult = pConfig->WriteInteger("KG3DENGINE", "SelectorScale", static_cast<int>(m_nScale));
	KG_PROCESS_ERROR(nResult);

	pConfig->Save(g_szConfigFilePath);

Exit0:
	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(m_pStencilRT);
	SAFE_RELEASE(m_pStencilDepth);
	SAFE_RELEASE(m_pRT);
	SAFE_RELEASE(m_pResult);
}


void KG3DSelector::OnLostDevice()
{
	UnInit();
}

void KG3DSelector::OnResetDevice()
{
	Init();
}

void KG3DSelector::ResetSelection()
{
	m_bSelected = FALSE;
}


KG3DModel* KG3DSelector::GetSelected()
{
	return m_pSelectedModel;
}

HRESULT KG3DSelector::OnRender()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	D3DLOCKED_RECT Rect;
	LPDIRECT3DSURFACE9 pResultColor = NULL;
	LPDIRECT3DSURFACE9 pStencilRT = NULL;
	D3DVIEWPORT9 vp;
	D3DVIEWPORT9 vpNew;
	KG3DRenderState RenderState;
	DWORD dwSelectIndex = 0xff;
	g_pd3dDevice->GetViewport(&vp);
		
	vpNew = vp;
	vpNew.X /= m_nScale;
	vpNew.Y /= m_nScale;
	vpNew.Width /= m_nScale;
	vpNew.Height /= m_nScale;
	
	m_dwIndex = 0;//Stencil Index清零
	KG_PROCESS_SUCCESS(!m_vecModels.size());
		
	hResult = g_pd3dDevice->GetRenderTarget(0, &m_pRTSave);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->GetDepthStencilSurface(&m_pDepthSave);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = m_pStencilRT->GetSurfaceLevel(0, &pStencilRT);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetRenderTarget(0, pStencilRT);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetDepthStencilSurface(m_pStencilDepth);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 1L);
	KG_COM_PROCESS_ERROR(hResult);
	
	g_pd3dDevice->SetViewport(&vpNew);

	RenderModelList();
	
	if (0)
	{
		D3DXSaveTextureToFile("d:\\test.tga", D3DXIFF_TGA, m_pStencilRT, NULL);
	}

	hResult = g_pd3dDevice->SetTexture(0, m_pStencilRT);
	KG_COM_PROCESS_ERROR(hResult);

	g_pd3dDevice->SetTexture(1, NULL);

	hResult = g_pd3dDevice->SetDepthStencilSurface(NULL);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetRenderTarget(0, m_pRT);
	KG_COM_PROCESS_ERROR(hResult);
	
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	float fU = float(m_MousePoint.x) / m_nWidth ;
	float fV = float(m_MousePoint.y) / m_nHeight;

	ScreenRect[0].vec2UV = D3DXVECTOR2(fU, fV);
	ScreenRect[1].vec2UV = D3DXVECTOR2(fU, fV);
	ScreenRect[2].vec2UV = D3DXVECTOR2(fU, fV);
	ScreenRect[3].vec2UV = D3DXVECTOR2(fU, fV);

	hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 1L);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetFVF(SelectorVertex::dwFVF);
	KG_COM_PROCESS_ERROR(hResult);

	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	hResult = g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, ScreenRect, sizeof(SelectorVertex));
	KG_COM_PROCESS_ERROR(hResult);

	hResult = m_pResult->GetSurfaceLevel(0, &pResultColor);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->GetRenderTargetData(m_pRT, pResultColor);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = pResultColor->LockRect(&Rect, NULL, 0);
	KG_COM_PROCESS_ERROR(hResult);

	BYTE *pBit = (BYTE*)Rect.pBits;
	dwSelectIndex = *(pBit + 2);

	pResultColor->UnlockRect();
	KG_COM_PROCESS_ERROR(hResult);

	if (dwSelectIndex != 0xff)
	{
		_ASSERTE(dwSelectIndex < m_vecModels.size());
		m_pSelectedModel = m_vecModels[dwSelectIndex];
	}
	else
	{
		//模糊选择, 在所有的模型都没有精确选中的时候， 选中离镜头最近的那一个
		D3DXVECTOR3 vec3CamPos;
		float fNear = 9999999999.f;
		DWORD dwNear = 0xffffffff;
		g_cGraphicsTool.GetCamera()->GetPosition(&vec3CamPos);
		for (size_t i = 0; i < m_vecModels.size(); i++)
		{
			D3DXVECTOR3 vec3Length = vec3CamPos - D3DXVECTOR3(m_vecModels[i]->m_matWorld._41, 
				m_vecModels[i]->m_matWorld._42, 
				m_vecModels[i]->m_matWorld._43);
			
			float fLength = vec3Length.x * vec3Length.x + vec3Length.y * vec3Length.y + vec3Length.z * vec3Length.z;
			if (fLength < fNear)
			{
				fNear = fLength;
				dwNear = static_cast<DWORD>(i);
			}
		}
		m_pSelectedModel = m_vecModels[dwNear];
	}

	m_bSelected = TRUE;

Exit1:
	hRetCode = S_OK;
Exit0:
	
	if (!m_bSelected)
	{
		m_pSelectedModel = NULL;
	}
		
	m_vecModels.clear();
	SAFE_RELEASE(pStencilRT);
	SAFE_RELEASE(pResultColor);
	if (m_pRTSave)
	{
		g_pd3dDevice->SetRenderTarget(0, m_pRTSave);
	}
	if (m_pDepthSave)
	{
		g_pd3dDevice->SetDepthStencilSurface(m_pDepthSave);
		g_pd3dDevice->SetViewport(&vp);
	}
	
	SAFE_RELEASE(pResultColor);
	SAFE_RELEASE(m_pRTSave);
	SAFE_RELEASE(m_pDepthSave);
	return hRetCode;
}


void KG3DSelector::Push(KG3DModel *pModel)
{
	pModel->SetSelectIndex(m_dwIndex);
	m_dwIndex += 1;
	m_vecModels.push_back(pModel);
}

extern LPD3DXEFFECT g_pSkinShader;
void KG3DSelector::RenderModelList()
{
	for (size_t i = 0; i < m_vecModels.size(); i++)
	{
		if (m_vecModels[i])
		{
			m_vecModels[i]->RenderForSelector(0, NULL);
		}
	}

	if (g_pSkinShader)
	{
		g_pSkinShader->End();
		g_pSkinShader = NULL;
	}
}

