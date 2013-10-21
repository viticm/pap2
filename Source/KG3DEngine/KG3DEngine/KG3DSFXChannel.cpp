////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXChannel.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-10-20 16:11:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSFXChannel.h"
#include "KG3DModel.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTextureTable.h"
#include "KG3DSFX.h"
#include "kg3drenderstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bClient; 

KG3DSFXChannel::KG3DSFXChannel(KG3DSFX* pSFX, int nPieceCount)
{
    ASSERT(nPieceCount > 0); 
    m_nPieceCount = nPieceCount;
   
    m_pTexture     = NULL;
    m_pParentSFX   = pSFX;
    m_fLength      = 1000.0f;
    m_fWidth       = 50.0f;
    m_fUSpeed      = 0.0f;
    m_fUOffset     = 0.0f;
    m_fVUpdateFcy  = 0.05f;
    m_nVCount      = 4;
    m_fCounter     = 0.0f;
    m_nCurVIndex   = 0;
    m_fAngelStride = 0.0f;

    m_nBlendMode   = 0;
	m_dwPrevTime    = 0;

    m_lpVertexBuffet = NULL;
	
    D3DXVec3Normalize(&m_vSourPos, &m_vSourPos);
    D3DXVec3Normalize(&m_vDestPos, &m_vDestPos);

    //for (int i = 0; i <= 5; i++)
   //     m_vVertexs[i] = D3DXVECTOR3(0.f, 0.f, 0.f);

    ReSet();
}


KG3DSFXChannel::~KG3DSFXChannel(void)
{
    SAFE_RELEASE(m_lpVertexBuffet);
    SAFE_RELEASE(m_pTexture);
}

HRESULT KG3DSFXChannel::SetBlendMode(int nMode)
{
    ASSERT(nMode >= 0 && nMode <= 2);
    m_nBlendMode = nMode;
    return S_OK;
}

HRESULT KG3DSFXChannel::OnLostDevice()
{
    SAFE_RELEASE(m_lpVertexBuffet);
    return S_OK;
}

HRESULT KG3DSFXChannel::OnResetDevice()
{
    ReSet();

    return S_OK;
}

HRESULT KG3DSFXChannel::ReSet()
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	ASSERT(m_nPieceCount > 0); 
	SAFE_RELEASE(m_lpVertexBuffet);

	KGLOG_PROCESS_ERROR(GetCurrentThreadId() == g_dwMainThreadID);

	hRetCode = g_pd3dDevice->CreateVertexBuffer(
		sizeof(VFormat::_Faces_Diffuse_Texture1) * 6 * m_nPieceCount,
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3DFVF_Faces_Diffuse_Texture1,
		D3DPOOL_DEFAULT,
		&m_lpVertexBuffet,
		NULL
		);
	if (FAILED(hRetCode))
	{
		SAFE_RELEASE(m_lpVertexBuffet);
		KG_PROCESS_ERROR(FALSE);
	}

	m_fAngelStride = D3DX_PI / m_nPieceCount;
	m_fVStride = 1.0f / m_nVCount;

	hResult = S_OK;
Exit0:
	return hResult;
}

//#if 1
//    D3DXVECTOR3 scFront;
//    D3DXVECTOR3 scFront_v;
//    D3DXVECTOR3 scUp;
//#endif 


HRESULT KG3DSFXChannel::FrameMove()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DXMATRIX  matTrans;
    D3DXVECTOR3 vRight;
    D3DXVECTOR3 vFront;
    D3DXVECTOR3 vUp;
    float       fHalfWidth;
    float       fChannelLength;
    float       fVPenU;
	DWORD	    dwNowTime = 0;
	float		fDelayTime;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    VFormat::_Faces_Diffuse_Texture1* pBuffet = NULL;

    KG_PROCESS_ERROR(m_lpVertexBuffet);
    KG_PROCESS_ERROR(pCamera);
    KG_PROCESS_ERROR(m_pParentSFX);

    fHalfWidth = m_fWidth / 2;

    vFront = pCamera->GetCameraFront();
    vRight = m_vSourPos - m_vDestPos;

    fChannelLength = D3DXVec3Length(&vRight);
    KG_PROCESS_ERROR(fChannelLength > 5.0f);

    D3DXVec3Normalize(&vFront, &vFront);
    D3DXVec3Normalize(&vRight, &vRight);

//#if 1
//    scFront   = pCamera->GetCameraFront();
//    scFront_v = vFront;
//#endif

    D3DXVec3Cross(&vUp, &vFront, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

	if (m_dwPrevTime == 0)
		m_dwPrevTime  = g_cGraphicsTool.GetNowTime();
	dwNowTime = g_cGraphicsTool.GetNowTime();

	fDelayTime  = (dwNowTime - m_dwPrevTime) / 1000.f; 
    m_fCounter +=  fDelayTime;
	m_dwPrevTime = dwNowTime;

    if (m_fCounter > m_fVUpdateFcy)
    {
        m_nCurVIndex += 1;
        if (m_nCurVIndex >= m_nVCount)
            m_nCurVIndex = 0;
        m_fCounter = 0.0f;
    }

    hRetCode = m_lpVertexBuffet->Lock(
        0, sizeof(VFormat::_Faces_Diffuse_Texture1) * 6 * m_nPieceCount, (void**)&pBuffet, D3DLOCK_DISCARD
        );
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    fVPenU = m_fWidth / m_fLength * 1.0f;
    for (int i = 0; i < m_nPieceCount; i++)
    {
        float fAngle  = i * m_fAngelStride;
        float fVMove  = m_nCurVIndex * m_fVStride;

        m_fUOffset += m_fUSpeed * fDelayTime;
        D3DXMatrixRotationAxis(&matTrans, &vRight, fAngle);
        D3DXVec3TransformNormal(&vFront, &vUp, &matTrans);
        D3DXVec3Normalize(&vFront, &vFront);

//#if 1
//        scUp = vFront;
//#endif

        // p0
        pBuffet[0 + i * 6].p = m_vSourPos;
        pBuffet[0 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[0 + i * 6].tu1 = 0 + m_fUOffset;
        pBuffet[0 + i * 6].tv1 = fVMove + 0.5f * m_fVStride;

        // p1
        pBuffet[1 + i * 6].p = m_vSourPos + vFront * fHalfWidth - vRight * m_fWidth;
        pBuffet[1 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[1 + i * 6].tu1 = 0 + m_fUOffset + fVPenU;
        pBuffet[1 + i * 6].tv1 = fVMove;

        // p2
        pBuffet[2 + i * 6].p = m_vSourPos - vFront * fHalfWidth - vRight * m_fWidth;
        pBuffet[2 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[2 + i * 6].tu1 = 0 + m_fUOffset + fVPenU;
        pBuffet[2 + i * 6].tv1 = fVMove + m_fVStride;

        // p3
        pBuffet[3 + i * 6].p = m_vDestPos + vFront * fHalfWidth + vRight * m_fWidth;
        pBuffet[3 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[3 + i * 6].tu1 = fChannelLength / m_fLength + m_fUOffset - fVPenU;
        pBuffet[3 + i * 6].tv1 = fVMove;

        // p4
        pBuffet[4 + i * 6].p = m_vDestPos - vFront * fHalfWidth + vRight * m_fWidth;
        pBuffet[4 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[4 + i * 6].tu1 = fChannelLength / m_fLength + m_fUOffset - fVPenU;
        pBuffet[4 + i * 6].tv1 = fVMove + m_fVStride;

        // p5
        pBuffet[5 + i * 6].p = m_vDestPos;
        pBuffet[5 + i * 6].diffuse = 0xFFFFFFFF;
        pBuffet[5 + i * 6].tu1 = fChannelLength / m_fLength + m_fUOffset;
        pBuffet[5 + i * 6].tv1 = fVMove + 0.5f * m_fVStride;

    }

    //// cals abbox vertex
    //if (m_nPieceCount)
    //{
    //    for (int i = 0; i <= 5; i++)
    //        m_vVertexs[i] = pBuffet[i].p;

    //    if (m_nNeedUpdateBBox && m_pParentSFX)
    //    {
    //        m_pParentSFX->ComputeBBox();
    //        m_nNeedUpdateBBox = FALSE;
    //    }
    //}

    hRetCode = m_lpVertexBuffet->Unlock();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;       
}

HRESULT KG3DSFXChannel::Render(float fCurrentFrame)
{
    HRESULT hResult  = E_FAIL;

    GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
    KG3DRenderState RenderState;

    D3DXMATRIX matWorldSave;
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);

	KG_PROCESS_ERROR(m_lpVertexBuffet);

	if (m_pParentSFX && m_pParentSFX->GetPlayAnimation() != SFX_AP_PAUSE)
		FrameMove();

    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    RenderState.SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);    
    RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);	
    RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    RenderState.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    RenderState.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	BOOL bTextureReady = FALSE;

    if (m_pTexture)
	{
		m_pTexture->SetTexture(g_pd3dDevice, 0);
		bTextureReady = m_pTexture->IsResourceReady();
	}
    else
        g_pd3dDevice->SetTexture(0, NULL);

    g_pd3dDevice->SetTexture(1, NULL);
    RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    RenderState.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
    RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
    RenderState.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

    RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    switch (m_nBlendMode)
    {
    case 0 :
        RenderState.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
        RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;
    case 1 :
        RenderState.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR);
        RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    case 2 :
        RenderState.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
        RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;
    default :
        ASSERT(0);
        break;
    }
    g_pd3dDevice->SetStreamSource(
        0, m_lpVertexBuffet, 0, sizeof(VFormat::FACES_DIFFUSE_TEXTURE1) 
    );
    g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);

	if(bTextureReady)
	{
		for (int i = 0; i < m_nPieceCount; i++)
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 6, 4);
	}


//#if 1
//    D3DXVECTOR3 v1 = m_vSourPos;
//    D3DXVECTOR3 v2 = m_vSourPos + scFront/*g_cGraphicsTool.GetCamera()->GetCameraFront()*/ * 50.f;
//    g_cGraphicsTool.DrawLine(
//        &v1, &v2, 0xFF0000FF, 0xFF0000FF
//        );
//    v1 = m_vSourPos;
//    v2 = m_vSourPos + scUp * 50.f;
//    g_cGraphicsTool.DrawLine(
//        &v1, &v2, 0xFF00FFFF, 0xFF00FFFF
//        );
//   /* v1 = m_vSourPos;
//    v2 = m_vSourPos + scFront_v * 50.f;
//    g_cGraphicsTool.DrawLine(
//        &v1, &v2, 0xFFFFFF00, 0xFFFFFF00
//        );*/
//
//#endif

    
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::UpdateEffectPos(D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget)
{
    ASSERT(vSource != vTarget);

    if (m_vSourPos != vSource || m_vDestPos != vTarget)
	{
		if (m_pParentSFX)
			m_pParentSFX->ComputeBBox();
	}
  /*      m_nNeedUpdateBBox = TRUE;
    else
        m_nNeedUpdateBBox = FALSE;*/

    m_vSourPos = vSource;
    m_vDestPos = vTarget;

    return S_OK;
}

HRESULT KG3DSFXChannel::SetTexture(LPCTSTR szFileName)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    SAFE_RELEASE(m_pTexture);
    hRetCode = g_cTextureTable.LoadResourceFromFile(
        szFileName, 
        0,
        TEXOP_RELOAD, 
        (IKG3DResourceBase**)&m_pTexture
    );
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::GetTexture(IEKG3DTexture** pTexture)
{
	_ASSERTE(pTexture);
	if (pTexture)
	{
		*pTexture = GetTexture();
		return S_OK;
	}
	return E_FAIL;
}


KG3DTexture* KG3DSFXChannel::GetTexture()
{
    return m_pTexture;
}

HRESULT KG3DSFXChannel::SetPieceCount(int nCount)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(nCount > 0);
    m_nPieceCount = nCount;

    hRetCode = ReSet();
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::SetPieceWidth(float fWidte)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(fWidte > 0.0f);
    m_fWidth = fWidte;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::SetPieceLength(float fLength)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(fLength > 0.0f);
    m_fLength = fLength;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::SetUSpeed(float fUSpeed)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(fUSpeed >= 0.0f);
    m_fUSpeed = fUSpeed;

    hResult = S_OK;
Exit0:
    return hResult;  
}

HRESULT KG3DSFXChannel::SetVUpdateFcy(float fVUpdateFcy)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(fVUpdateFcy > 0.0f);
    m_fVUpdateFcy = fVUpdateFcy;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSFXChannel::SetVPieceCount(int nCount)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(nCount > 0);
    m_nVCount = nCount;

    hRetCode = ReSet();
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

int KG3DSFXChannel::GetPieceCount()
{
    return m_nPieceCount;
}

float KG3DSFXChannel::GetPieceWidth()
{
    return m_fWidth;
}

float KG3DSFXChannel::GetPieceLength()
{
    return m_fLength;
}

float KG3DSFXChannel::GetUSpeed()
{
    return m_fUSpeed;
}

float KG3DSFXChannel::GetVUpdateFcy()
{
    return m_fVUpdateFcy;
}

int KG3DSFXChannel::GetVPieceCount()
{
    return m_nVCount;
}
