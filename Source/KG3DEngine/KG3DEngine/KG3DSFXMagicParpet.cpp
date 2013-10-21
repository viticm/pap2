
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXMagicParpet.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-07-21 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXMagicParpet.h"
#include "KG3DTextureTable.h"
#include "KG3DScene.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSFX.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DKeyFrameDataBase.h"
#include "KG3DGraphiceEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_FOOTPRINT_NUM 12
extern BOOL g_bClient;

inline float _hundredlize(float fValue, float& fOffset)
{
	float fHunder = 0;
	if (fValue > 0.0f)
		fHunder = static_cast<float>((static_cast<int>(fValue) + 50) / 100 * 100);
	else if (fValue < 0.0f)
		fHunder = static_cast<float>((static_cast<int>(fValue) - 50) / 100 * 100);
	else
		fHunder = fValue;
	fOffset = fHunder - fValue;

	return fHunder;
}

inline int _cursorysamepos(D3DXMATRIX matA, D3DXMATRIX matB)
{
	if (
		(static_cast<int>(matA._41) == static_cast<int>(matB._41)) &&
		(static_cast<int>(matA._43) == static_cast<int>(matB._43)) 
	)
		return true;
	else
		return false;
}

KG3DSFXMagicParpet::KG3DSFXMagicParpet(KG3DSFX *pSFX, DWORD dwType)
{
    m_pSFX		= pSFX;
    m_pTexture  = NULL;
    m_lpVB		= NULL;
    m_lpIB		= NULL;
    m_fOffset	= 1.0f;
    m_fHeightLimit	 = 400.0f;
	m_nCellCountLine = 2;
    m_nRelRadius  = 50;
	m_nRadius	  = 50;
	m_nRelRadius  = -1;
	m_fUVZoom     = 0.0f;
	m_fUVStep	  = 0.0f;
	m_fUVRotation = 0.0f;
	D3DXMatrixIdentity(&m_matSaveSFXWorld);
	m_fLife = 5000; 
    m_nCurCellCountLine = m_nCellCountLine;
	m_fAngel = 0;
	//m_vPosition = D3DXVECTOR3(0,0,0);
    m_dwType = 0xffffffff;
    m_CurrColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    m_dwFaceType = AOE_FACE_NONE;
    m_dwType = dwType;
    m_nTexCullX = 1;
    m_nTexCullY = 1;
    m_nCurrTexX = 1;
    m_nCurrTexY = 1;
    m_texSpeed = (float)pSFX->GetTotalFrameNum();

    m_tlRadius.InsertKeyFrame(0, 50);
    m_tlColor.InsertKeyFrame(0, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));

#if 0
    m_tlRadius.InsertKeyFrame(0, 200);
    m_tlRadius.InsertKeyFrame(50, 1000);
    m_tlRadius.InsertKeyFrame(100, 200);

    m_tlColor.InsertKeyFrame(0, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
    m_tlColor.InsertKeyFrame(50, D3DXCOLOR(0.f, 0.f, 1.f, 1.f));
    m_tlColor.InsertKeyFrame(100, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
#endif
}

KG3DSFXMagicParpet::~KG3DSFXMagicParpet(void)
{
    Clear(true);
}

HRESULT KG3DSFXMagicParpet::OnLostDevice()
{
    Clear(false);

    return S_OK;
}

HRESULT KG3DSFXMagicParpet::OnResetDevice()
{
    SetType(m_dwType, true);
    return S_OK;
}

void KG3DSFXMagicParpet::SetTexCull(int x, int y)
{
    ASSERT(x > 0 && y > 0);
    m_nTexCullX = x;
    m_nTexCullY = y;
}

void KG3DSFXMagicParpet::SetTexSpeed(float speed)
{
    m_texSpeed = speed;
}

void KG3DSFXMagicParpet::GetTexCull(int* x, int* y)
{
    ASSERT(x && y);

    (*x) = m_nTexCullX;
    (*y) = m_nTexCullY;
}

float KG3DSFXMagicParpet::GetTexSpeed()
{
    return m_texSpeed;
}

void KG3DSFXMagicParpet::Clear(bool bClearTex)
{
    if (bClearTex)
        SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_lpIB);
    SAFE_RELEASE(m_lpVB);
}

bool KG3DSFXMagicParpet::NeedFillVB()
{
    D3DXVECTOR3 vWorldPos;

    if (m_dwType == AOE_PROJECT || m_dwType == AOE_SELECTOR)
        return false;

    vWorldPos = GetWorldPos();

    if (vWorldPos != m_vWorldPosSave)
    {
        m_vWorldPosSave = vWorldPos;
        return true;
    }

    return false;
}

HRESULT KG3DSFXMagicParpet::Update(float fCurFrame)
{
    float fScal = 1.f;

    if (m_dwType == AOE_PROJECT)
        m_tlRadius.GetData(&m_nBaseRadius, fCurFrame);
    m_tlColor.GetData(&m_CurrColor, fCurFrame);
    
    if (m_pSFX)
        fScal = (m_pSFX->m_Srt.vScanl.x + m_pSFX->m_Srt.vScanl.z) * 0.5f;
    SetRadius(static_cast<int>(GetBaseRaduis() * fScal));

    int nTexIndex = (int)(fCurFrame / m_texSpeed) % (m_nTexCullX * m_nTexCullY);
    m_nCurrTexY = nTexIndex / m_nTexCullX;
    m_nCurrTexX = nTexIndex % m_nTexCullX;

    if (m_dwType != AOE_PROJECT && m_dwType != AOE_SELECTOR)
    {
        KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

        if (m_dwType == AOE_FOOTPRINT)
            m_pSFX->m_dwTotalFrameNum = 0xffffffff;

        D3DXVECTOR3 vFront,vCross;
        D3DXVECTOR3 vDiret = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

        if (m_dwFaceType == AOE_FACE_EYES && pCamera)
        {
            vFront = pCamera->GetCameraFront();
            vFront.y = 0.0f;
            D3DXVec3Normalize(&vFront, &vFront);
            m_fAngel = acosf(D3DXVec3Dot(&vFront, &vDiret));
            D3DXVec3Cross(&vCross, &vFront, &vDiret);
            if (vCross.y < 0.0f)
                m_fAngel = -m_fAngel;
        }
        else if (m_dwFaceType == AOE_FACE_PLAYER && m_pSFX)
        {
            D3DXMATRIX matSFXWorld;
            matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
            vFront = D3DXVECTOR3(-matSFXWorld._31,-matSFXWorld._32,-matSFXWorld._33);
            D3DXVec3Normalize(&vFront, &vFront);
            m_fAngel = acosf(D3DXVec3Dot(&vFront, &vDiret));
            D3DXVec3Cross(&vCross, &vFront, &vDiret);
            if (vCross.y < 0.0f)
                m_fAngel = -m_fAngel;
        }

        if (NeedFillVB())
        {
            VFormat::_Faces_Diffuse_Texture1 *pData = NULL;
            if (m_lpVB && SUCCEEDED(m_lpVB->Lock(0, 0, (VOID**)&pData, D3DLOCK_DISCARD)))
            {
				DWORD dwMax = (m_nCellCountLine + 1) * (m_nCellCountLine + 1);
                FillVertexBuffer(pData,dwMax);
                m_lpVB->Unlock();
            }
        }
    }

	return S_OK;

    /*
	if (m_dwType == AOE_FOOTPRINT)
		m_pSFX->m_dwTotalFrameNum = 0xffffffff;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    D3DXVECTOR3 vFront,vCross;
    D3DXVECTOR3 vDiret = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    if (m_dwFaceType == AOE_FACE_EYES && pCamera)
    {
        vFront = pCamera->GetCameraFront();
        vFront.y = 0.0f;
        D3DXVec3Normalize(&vFront, &vFront);
        m_fAngel = acosf(D3DXVec3Dot(&vFront, &vDiret));
        D3DXVec3Cross(&vCross, &vFront, &vDiret);
        if (vCross.y < 0.0f)
            m_fAngel = -m_fAngel;
    }
    else if (m_dwFaceType == AOE_FACE_PLAYER && m_pSFX)
    {
        D3DXMATRIX matSFXWorld;
        matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
        vFront = D3DXVECTOR3(-matSFXWorld._31,-matSFXWorld._32,-matSFXWorld._33);
        D3DXVec3Normalize(&vFront, &vFront);
        m_fAngel = acosf(D3DXVec3Dot(&vFront, &vDiret));
        D3DXVec3Cross(&vCross, &vFront, &vDiret);
        if (vCross.y < 0.0f)
            m_fAngel = -m_fAngel;
        hr = g_pd3dDevice->CreateIndexBuffer(
            sizeof(WORD) * m_nCellCountLine * m_nCellCountLine * 6,
            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_lpIB, NULL);
        KG_COM_PROCESS_ERROR(hr);
		m_nCurCellCountLine = m_nCellCountLine;
        hr = m_lpIB->Lock(0, 0, (VOID**)&pIndex, 0);
        KG_COM_PROCESS_ERROR(hr);
		bIBLocked = TRUE;

        FillIndexBuffer(pIndex,m_nCellCountLine);

        hr = m_lpIB->Unlock();
        KG_COM_PROCESS_ERROR(hr);
		bIBLocked = FALSE;
		
    }

    VFormat::_Faces_Diffuse_Texture1 *pData = NULL;
    if (m_lpVB && SUCCEEDED(m_lpVB->Lock(0, 0, (VOID**)&pData, D3DLOCK_DISCARD)))
    {
        FillVertexBuffer(pData);
        m_lpVB->Unlock();
    }

*/

}
HRESULT KG3DSFXMagicParpet::Render()
{
	if (g_bClient)
	{
		if (m_dwType == AOE_FOOTPRINT)
			UpdateFootprint();	
		else
			RenderAOE();
	}
	else
	{
		RenderAOE();
		if (m_dwType == AOE_FOOTPRINT)
			UpdateFootprint();
	}

    return S_OK;
}


HRESULT KG3DSFXMagicParpet::RenderAOE()
{
    HRESULT hr = E_FAIL;
	
	D3DXMATRIX  matTexSaved;
	DWORD	    dwTTFF = 0;
	DWORD	    dwADDRESSU;
	DWORD	    dwADDRESSV;

    if (m_dwType == AOE_PROJECT || m_dwType == AOE_SELECTOR)
        return S_OK;

	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &dwADDRESSU);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSV, &dwADDRESSV);

	if (g_cGraphicsEngine.GetD3DCaps().TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	}
	else
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	//if (m_nOptionFcaeTo/*m_nFaceToCamera && pCamera*/)
	if (m_dwFaceType == AOE_FACE_PLAYER || m_dwFaceType == AOE_FACE_EYES)
    {
		g_pd3dDevice->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwTTFF);
		g_pd3dDevice->GetTransform(D3DTS_TEXTURE0, &matTexSaved); 

		D3DXMATRIX matTranA;
		D3DXMATRIX matTranB;
		D3DXMATRIX matTexRt; 
		D3DXMATRIX matTotal;
		D3DXMatrixIdentity(&matTranA);
		matTranA._31 =  0.5f;
		matTranA._32 =  0.5f;
		D3DXMatrixIdentity(&matTranB);
		matTranB._31 = -0.5f;
		matTranB._32 = -0.5f;
		D3DXMatrixRotationZ(&matTexRt, m_fAngel);
		matTotal = matTranB * matTexRt * matTranA;
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTotal);
	}

	hr = g_pd3dDevice->SetStreamSource(0, m_lpVB, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	KG_COM_PROCESS_ERROR(hr);
	hr = g_pd3dDevice->SetIndices(m_lpIB);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetTexture(0, m_pTexture ? m_pTexture->GetTexture() : NULL);
	KG_COM_PROCESS_ERROR(hr);

	BOOL bTextureReady = FALSE;
	if(m_pTexture)
		bTextureReady = m_pTexture->IsResourceReady();

	if(bTextureReady)
	{
		hr = g_pd3dDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, 0, 0, 
			(m_nCellCountLine + 1) * (m_nCellCountLine + 1), 0, 2 * m_nCellCountLine * m_nCellCountLine
			);
		KG_COM_PROCESS_ERROR(hr);	
	}
	
    hr = S_OK;
Exit0:
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, dwADDRESSU);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, dwADDRESSV);

	if(m_dwFaceType == AOE_FACE_EYES || m_dwFaceType == AOE_FACE_PLAYER)
	{
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dwTTFF);
		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTexSaved);
	}
    return hr;
}

void KG3DSFXMagicParpet::CalcParam()
{
    float fOffset = 0.0f;

    m_nRadius = static_cast<int>(_hundredlize(static_cast<float>(m_nRelRadius), fOffset));

    if (fOffset < 0)
        m_nRadius += 100;
    m_nRadius += 200;

    m_nCellCountLine = m_nRadius / 100 * 2;

    m_fUVZoom = (static_cast<float>(m_nRadius) / static_cast<float>(m_nRelRadius) - 1) / 2;
    m_fUVStep = (fabs(m_fUVZoom) * 2 + 1) / m_nCellCountLine;
}

void KG3DSFXMagicParpet::SetRadius(int nRadius, int bBaseRadius /* = false */)
{
	int nResult  = false;

	KG_PROCESS_ERROR(nRadius < 1000);
	KG_PROCESS_ERROR(nRadius > 0);

    if (nRadius == m_nRelRadius)
        return;

	m_nRelRadius = nRadius;	

	if (bBaseRadius)
		m_nBaseRadius = nRadius;

    if (m_dwType != AOE_PROJECT && m_dwType != AOE_SELECTOR)
    {
        SAFE_RELEASE(m_lpVB);
        SAFE_RELEASE(m_lpIB);
    
        m_vWorldPosSave = D3DXVECTOR3(0.f, 1000000000.f, 0.f);
        CalcParam();
        CreateBuffer();
    }

	nResult = true;
Exit0 :
	return;
}

HRESULT KG3DSFXMagicParpet::SetTexture(const char cszTextureName[])
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

    KG3DTexture* pTexture = NULL;

	hRetCode = g_cTextureTable.LoadResourceFromFile(
		cszTextureName, 0, TEXOP_DISABLEDTX/*|TEXOP_RELOAD*/, (IKG3DResourceBase**)&pTexture
	);
	KG_COM_PROCESS_ERROR(hRetCode);

    SAFE_RELEASE(m_pTexture);
    m_pTexture = pTexture;

	hResult = S_OK;
Exit0:
	if (hResult != S_OK)
		KGLogPrintf(KGLOG_DEBUG, TEXT("can not open texture : %s, %s"), cszTextureName, KG_FUNCTION);
    return hResult;
}

void KG3DSFXMagicParpet::SetHeightLimit(float fLimit)
{
    m_fHeightLimit = fLimit;
}

void KG3DSFXMagicParpet::SetOffset(float fOffset)
{
    m_fOffset = fOffset;

    m_vWorldPosSave = D3DXVECTOR3(0.f, 10000000000.f, 0.f);
}

void KG3DSFXMagicParpet::FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData,DWORD dwMaxVertex)
{
	int nResult  = false;

	KG3DScene*	pCurScene  = NULL;
	D3DXMATRIX	matSFXWorld;
	D3DXVECTOR3	vPosition(0,0,0);
	D3DXVECTOR3	vPosVertex;
	
	float fLeft      = 0.0f;
	float fTop		 = 0.0f;
	float fRefHeight = 0.0f;
	int	  nStep      = 0;
	float fXO		 = 0;
	float fYO		 = 0;
	float fXOffset   = 0.0f;
	float fYOffset   = 0.0f;

	KGLOG_PROCESS_ERROR(pData);
	
	if(m_pSFX)
	{
		matSFXWorld = *m_pSFX->GetCurrentWorldMatrix();
		m_matSaveSFXWorld = matSFXWorld;
		//m_vPosition = D3DXVECTOR3(matSFXWorld._41,matSFXWorld._42,matSFXWorld._43);
		vPosition   = D3DXVECTOR3(
			_hundredlize(matSFXWorld._41, fXO),
			matSFXWorld._42,
			_hundredlize(matSFXWorld._43, fYO)
			);
		
	}
    
    pCurScene = g_cGraphicsTool.GetCurScene();
    KGLOG_PROCESS_ERROR(pCurScene);

	pCurScene->GetHeight(&vPosVertex, &vPosition);
	fRefHeight = vPosVertex.y;

	fLeft   = vPosition.x - m_nRadius;
	fTop    = vPosition.z + m_nRadius;

	fXOffset = fXO / 100.0f * m_fUVStep;
	fYOffset = fYO / 100.0f * m_fUVStep;
	
	/*DWORD dwAlpha = (DWORD)(m_fAlpha*255);*/

	for (int y = 0; y < m_nCellCountLine + 1; y++)
	{
		for (int x = 0; x < m_nCellCountLine + 1; x++)
		{
			KGLOG_PROCESS_ERROR(nStep<dwMaxVertex);

			vPosVertex.x = fLeft + x * 100;
			vPosVertex.z = fTop  - y * 100;
			vPosVertex.y = 0.0f;

			pCurScene->GetHeight(&vPosVertex, &vPosVertex);
			vPosVertex.y += m_fOffset;

			if (abs(vPosVertex.y - fRefHeight) > m_fHeightLimit)
				pData[nStep].diffuse = 0x00ffffff;
			else
			{
				pData[nStep].diffuse = 0xffffffff;//(dwAlpha<<24) | 0x00FFFFFF;
			}
			pData[nStep].p = vPosVertex;
			pData[nStep].tu1 =  -m_fUVZoom + x * m_fUVStep + fXOffset;
			pData[nStep].tv1 =  -m_fUVZoom + y * m_fUVStep - fYOffset;
			nStep++;
		}
	}

//Exit1:
	nResult = true;
Exit0:
	return;
}

void KG3DSFXMagicParpet::FillIndexBuffer(WORD *pData,int nCellCountLine)
{
	int nStep = 0;
	for (int y = 0; y < nCellCountLine; y++)
	{
		for (int x = 0; x < nCellCountLine; x++)
		{
			pData[nStep++] = static_cast<WORD>(x + y * (nCellCountLine + 1));
			pData[nStep++] = static_cast<WORD>(x + 1 + (y + 1) * (nCellCountLine + 1)); 
			pData[nStep++] = static_cast<WORD>(x + (y + 1) * (nCellCountLine + 1)); 

			pData[nStep++] = static_cast<WORD>(x + y * (nCellCountLine + 1));
			pData[nStep++] = static_cast<WORD>(x + 1 + y * (nCellCountLine + 1));
			pData[nStep++] = static_cast<WORD>(x + 1 + (y + 1) * (nCellCountLine + 1)); 
		}
	}
}

int KG3DSFXMagicParpet::GetBaseRaduis()
{
	return m_nBaseRadius;
}

int KG3DSFXMagicParpet::GetRadius()
{
    return m_nRelRadius;
    //int radius = m_nRelRadius;
    //if (m_pSFX)
    //    radius *= (m_pSFX->m_Srt.vScanl.x + m_pSFX->m_Srt.vScanl.y + m_pSFX->m_Srt.vScanl.z) / 3.f;
    //return radius;
}

float KG3DSFXMagicParpet::GetOffset()
{
    return m_fOffset;
}

int KG3DSFXMagicParpet::IsFaceCamera()
{
    return m_dwFaceType == AOE_FACE_EYES;
}

void KG3DSFXMagicParpet::EnableFaceCamera(int nFlag)
{
	//m_nOptionFcaeTo = nFlag; 
    m_dwFaceType = AOE_FACE_EYES;
}

float KG3DSFXMagicParpet::GetHeightLimit()
{
    return m_fHeightLimit;
}

IEKG3DTexture* KG3DSFXMagicParpet::GetTexture()
{
    return m_pTexture;
}

void KG3DSFXMagicParpet::SetAngle(float fAngle)
{
	m_fAngel = fAngle;
}
float KG3DSFXMagicParpet::GetAngle()
{
	return m_fAngel;
}
float KG3DSFXMagicParpet::GetLife()
{
	return m_fLife;
}
void KG3DSFXMagicParpet::SetLife(float fLife)
{
	m_fLife = fLife;
}

D3DXVECTOR3 KG3DSFXMagicParpet::GetWorldPos()
{
    D3DXMATRIX& matSfx = *m_pSFX->GetCurrentWorldMatrix();
    return D3DXVECTOR3(matSfx._41, matSfx._42, matSfx._43);
}

AABBOX KG3DSFXMagicParpet::CalcBBox()
{
    AABBOX box;
    D3DXVECTOR3 pos = GetWorldPos();
    float radius = (float)GetRadius();
    
    box.A = pos - D3DXVECTOR3(radius, m_fHeightLimit, radius);
    box.B = pos + D3DXVECTOR3(radius, m_fHeightLimit, radius);
    
    return box;
}

void KG3DSFXMagicParpet::GetInfo(D3DXVECTOR4* info)
{
    ASSERT(info);
	if(!info)
		return ;

	D3DXVECTOR3 vInfo3 = GetWorldPos();
	info->x = vInfo3.x;
	info->y = vInfo3.y;
	info->z = vInfo3.z;
    info->w = (float)GetRadius();

}

void KG3DSFXMagicParpet::GetTexFrameInfo(D3DXVECTOR4* info)
{
    ASSERT(info);

    info->x = 1.f / m_nTexCullX;
    info->y = 1.f / m_nTexCullY;
    info->z = m_nCurrTexX * info->x;
    info->w = m_nCurrTexY * info->y;
}

HRESULT KG3DSFXMagicParpet::CreateBuffer()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

	DWORD dwThreadID = ::GetCurrentThreadId();
	KG_PROCESS_ERROR(dwThreadID==g_dwMainThreadID);

    WORD *pIndex = NULL;

    if (!m_lpIB)
    {
        hRetCode = g_pd3dDevice->CreateIndexBuffer(sizeof(WORD) * m_nCellCountLine * m_nCellCountLine * 6,
            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_lpIB, NULL);
        KG_COM_PROCESS_ERROR(hRetCode);

        m_nCurCellCountLine = m_nCellCountLine;
        
        if (SUCCEEDED(m_lpIB->Lock(0, 0, (VOID**)&pIndex, 0)))
        {
            FillIndexBuffer(pIndex,m_nCellCountLine);
            m_lpIB->Unlock();
        }
    }

    if (!m_lpVB)
    {
		DWORD dwThreadID = GetCurrentThreadId();
		ASSERT(dwThreadID == g_dwMainThreadID);

        hRetCode = g_pd3dDevice->CreateVertexBuffer(
            sizeof(VFormat::_Faces_Diffuse_Texture1) * (m_nCellCountLine + 1) * (m_nCellCountLine + 1),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
            D3DFVF_Faces_Diffuse_Texture1, D3DPOOL_DEFAULT, &m_lpVB, NULL);
        KG_COM_PROCESS_ERROR(hRetCode);

        m_nCurCellCountLine = m_nCellCountLine;
    }

    hResult = S_OK;
Exit0 :
    return hResult;

}

void KG3DSFXMagicParpet::SetType(DWORD t, bool bForce)
{
    if (t == m_dwType && !bForce)
        return;

    Clear(false);

    switch (t)
    {
    case AOE_NORMAL :
    case AOE_FOOTPRINT :
        m_vWorldPosSave = D3DXVECTOR3(0.f, 1000000000.f, 0.f);
        CalcParam();
        CreateBuffer();
        break;
    case AOE_PROJECT :
    case AOE_SELECTOR :
        break;
    default :
        break;
    }

    m_dwType = t;
}

static float s_fFootprintRange[] = {2500.0f, 1500.0f, 800.0f};
static size_t   s_fFootprintCount[] = {30, 60, 80};
void KG3DSFXMagicParpet::FrameMove()
{
	list<FOOTPRINT>::iterator i = m_listFootprint.begin();
	while (i != m_listFootprint.end())
	{
		FOOTPRINT& node = *i;
		node.fLife = m_fLife - (g_cGraphicsTool.GetNowTime() - node.dwStartTime);
		if (node.fLife < 0)
		{	
			node.fLife = 0;
		}
		++i;
	}

	if (m_dwType == AOE_FOOTPRINT)
	{
		if(m_listFootprint.size()> 0)
		{
			FOOTPRINT& node = m_listFootprint.front();
			if (node.fLife <= 0)
				m_listFootprint.pop_front();
		}
		else
		{
			m_pSFX->m_dwTotalFrameNum = 2;
		}
	}
}

void KG3DSFXMagicParpet::UpdateFootprint()
{ 	
	HRESULT hRetCode = E_FAIL;
	int nFootSteopLODIndex = 0;
	list<FOOTPRINT>::iterator i;
	D3DXVECTOR3 vec3Dummy, vec3CamPos;
	hRetCode = g_cGraphicsTool.GetCurCameraData(&vec3CamPos, &vec3Dummy);
	KG_COM_PROCESS_ERROR(hRetCode);

	for (int i = sizeof(s_fFootprintCount) / sizeof(size_t) - 1; i >= 0; i--)
	{
		if (m_listFootprint.size() > s_fFootprintCount[i])
		{
			nFootSteopLODIndex = i;
			break;
		}
	}

	i = m_listFootprint.begin();
	while (i != m_listFootprint.end())
	{
		FOOTPRINT& node = *i;
		if (node.fLife > 0)
		{
			float fDistance = D3DXVec3Length(&(vec3CamPos - node.vPosition));
			if (fDistance < s_fFootprintRange[nFootSteopLODIndex])
			{
				RenderFootprint(node);
			}
		}
		++i;
	}
	
Exit0:
	;
}
HRESULT KG3DSFXMagicParpet::RenderFootprint(FOOTPRINT& footprint)
{
	HRESULT hr = E_FAIL;
	VFormat::_Faces_Diffuse_Texture1 *pData = NULL;
	WORD *pIndex = NULL;
	D3DXMATRIX  matTexSaved;
	DWORD	    dwTTFF = 0;
	DWORD	    dwADDRESSU = D3DTADDRESS_FORCE_DWORD;
	DWORD	    dwADDRESSV = D3DTADDRESS_FORCE_DWORD;
	BOOL   bReset = FALSE;
	
	if(m_pTexture && m_pTexture->IsResourceReady())
	{
		;
	}
	else
	{
		return S_OK;
	}
	//////////////////////////////////////////////////////////////////////////
	if (!m_lpIB || (m_nCurCellCountLine != footprint.nCellCountLine))
	{
		SAFE_RELEASE(m_lpIB);
		hr = g_pd3dDevice->CreateIndexBuffer(
			sizeof(WORD) * footprint.nCellCountLine * footprint.nCellCountLine * 6,
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_lpIB, NULL);
		KG_COM_PROCESS_ERROR(hr);
		m_nCurCellCountLine = footprint.nCellCountLine;
		hr = m_lpIB->Lock(0, 0, (VOID**)&pIndex, 0);
		KG_COM_PROCESS_ERROR(hr);

		FillIndexBuffer(pIndex,footprint.nCellCountLine);

		hr = m_lpIB->Unlock();
		KG_COM_PROCESS_ERROR(hr);

	}
	if ((!m_lpVB) || (m_nCurCellCountLine != footprint.nCellCountLine))
	{
		SAFE_RELEASE(m_lpVB);

		DWORD dwThreadID = GetCurrentThreadId();
		ASSERT(dwThreadID == g_dwMainThreadID);

		hr = g_pd3dDevice->CreateVertexBuffer(
			sizeof(VFormat::_Faces_Diffuse_Texture1) * (footprint.nCellCountLine + 1) * (footprint.nCellCountLine + 1),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
			D3DFVF_Faces_Diffuse_Texture1, D3DPOOL_DEFAULT, &m_lpVB, NULL);
		KG_COM_PROCESS_ERROR(hr);
		m_nCurCellCountLine = footprint.nCellCountLine;
	}
	
	hr = m_lpVB->Lock(0, 0, (VOID**)&pData, D3DLOCK_DISCARD);
	KG_COM_PROCESS_ERROR(hr);
	
	FillVertexBuffer(pData,footprint);

	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetStreamSource(0, m_lpVB, 0, sizeof(VFormat::_Faces_Diffuse_Texture1));
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	KG_COM_PROCESS_ERROR(hr);

	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSU, &dwADDRESSU);
	g_pd3dDevice->GetSamplerState(0, D3DSAMP_ADDRESSV, &dwADDRESSV);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

	g_pd3dDevice->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &dwTTFF);
	g_pd3dDevice->GetTransform(D3DTS_TEXTURE0, &matTexSaved);
	
	
	{
		
		D3DXMATRIX matTranA;
		D3DXMATRIX matTranB;
		D3DXMATRIX matTexRt; 
		D3DXMATRIX matTotal;
		D3DXMatrixIdentity(&matTranA);
		matTranA._31 =  0.5f;
		matTranA._32 =  0.5f;
		D3DXMatrixIdentity(&matTranB);
		matTranB._31 = -0.5f;
		matTranB._32 = -0.5f;
		D3DXMatrixRotationZ(&matTexRt, footprint.fAngel);
		matTotal = matTranB * matTexRt * matTranA;
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTotal);
		
		
	}
	bReset = TRUE;
	

	hr = g_pd3dDevice->SetIndices(m_lpIB);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->SetTexture(0, m_pTexture ? m_pTexture->GetTexture() : NULL);
	KG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, 0, 
		(footprint.nCellCountLine + 1) * (footprint.nCellCountLine + 1), 0, 2 * footprint.nCellCountLine * footprint.nCellCountLine
		);
	KG_COM_PROCESS_ERROR(hr);
	hr = S_OK;

Exit0:
	if (bReset)
	{
		
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, dwADDRESSU);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, dwADDRESSV);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, dwTTFF);
		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &matTexSaved);
	}
	return hr;


}
HRESULT KG3DSFXMagicParpet::NewOneFootprint()
{
	if (m_listFootprint.size()> MAX_FOOTPRINT_NUM)
		return S_OK;
	if(!m_pSFX)
		return E_FAIL;
	KG3DScene* pCurScene = NULL;
	float TerrainHeight = 0;
	D3DXVECTOR3 vPosition(0,0,0);
	pCurScene = static_cast<KG3DScene *>(g_cGraphicsTool.GetScene());
	KG_PROCESS_ERROR(pCurScene);
	vPosition = GetWorldPos();
	TerrainHeight = pCurScene->GetHeight(vPosition.x,vPosition.z);

	if (vPosition.y - TerrainHeight >= 100)
		vPosition.x = vPosition.y = vPosition.z = 0;
	{
		FOOTPRINT Footprint;

		Footprint.vPosition = vPosition;
		Footprint.fAngel = m_fAngel;
		Footprint.fLife = m_fLife;
		Footprint.nCellCountLine = m_nCellCountLine;
		Footprint.fUVStep = m_fUVStep;
		Footprint.fUVZoom = m_fUVZoom;
		Footprint.nRadius = m_nRadius;
		Footprint.dwStartTime = g_cGraphicsTool.GetNowTime();
		m_listFootprint.push_back(Footprint);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DSFXMagicParpet::FillVertexBuffer(VFormat::_Faces_Diffuse_Texture1* pData,FOOTPRINT& Footprint)
{
	int nResult  = false;

	KG3DScene*	pCurScene  = NULL;
	D3DXVECTOR3	vPosVertex;

	float fLeft      = 0.0f;
	float fTop		 = 0.0f;
	float fRefHeight = 0.0f;
	int	  nStep      = 0;
	float fXOffset   = 0.0f;
	float fYOffset   = 0.0f;
	bool  bNeedSaveVertexHeight = !Footprint.CellVertexHeight.size();
	
	pCurScene = static_cast<KG3DScene *>(g_cGraphicsTool.GetScene());
	KG_PROCESS_ERROR(pCurScene);

	if (bNeedSaveVertexHeight)
	{
		D3DXVECTOR3	vPosition;
		float fXO		 = 0;
		float fYO		 = 0;
		vPosition = D3DXVECTOR3(
			_hundredlize(Footprint.vPosition.x, fXO),
			Footprint.vPosition.y,
			_hundredlize(Footprint.vPosition.z, fYO)
			);

		pCurScene->GetHeight(&vPosVertex, &vPosition);
		fRefHeight = vPosVertex.y;
		Footprint.vPosition = vPosVertex;

		fLeft   = vPosition.x - Footprint.nRadius;
		fTop    = vPosition.z + Footprint.nRadius;
		
		fXOffset = fXO / 100.0f * Footprint.fUVStep;
		fYOffset = fYO / 100.0f * Footprint.fUVStep;

		Footprint.vParam = D3DXVECTOR4(fLeft, fTop, fXOffset, fYOffset);

		Footprint.CellVertexHeight.resize((Footprint.nCellCountLine + 1) * (Footprint.nCellCountLine + 1));
	}
	else
	{
		vPosVertex = Footprint.vPosition;
		fRefHeight = Footprint.vPosition.y;

		fLeft = Footprint.vParam.x;
		fTop  = Footprint.vParam.y;
		fXOffset = Footprint.vParam.z;
		fYOffset = Footprint.vParam.w;
	}
	
	DWORD dwAlpha = (DWORD)(Footprint.fLife / m_fLife *255);
	for (int y = 0; y < Footprint.nCellCountLine + 1; y++)
	{
		for (int x = 0; x < Footprint.nCellCountLine + 1; x++)
		{
			vPosVertex.x = fLeft + x * 100;
			vPosVertex.z = fTop  - y * 100;
			vPosVertex.y = 0.0f;

			if (bNeedSaveVertexHeight)
			{
				pCurScene->GetHeight(&vPosVertex, &vPosVertex);
				Footprint.CellVertexHeight[nStep] = vPosVertex.y;
			}
			else
			{
				vPosVertex.y = Footprint.CellVertexHeight[nStep];
			}

			vPosVertex.y += m_fOffset;

			if (abs(vPosVertex.y - fRefHeight) > m_fHeightLimit)
				pData[nStep].diffuse = 0x00ffffff;
			else
			{
				pData[nStep].diffuse = (dwAlpha<<24) | 0x00FFFFFF;
			}
			pData[nStep].p = vPosVertex;
			pData[nStep].tu1 =  -Footprint.fUVZoom + x * Footprint.fUVStep + fXOffset;
			pData[nStep].tv1 =  -Footprint.fUVZoom + y * Footprint.fUVStep - fYOffset;
			nStep++;
		}
	}

	//Exit1:
	nResult = true;
Exit0:
	return;
}
