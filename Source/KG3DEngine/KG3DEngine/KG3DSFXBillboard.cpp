////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXBillboard.h"
#include "KG3DGraphicsTool.h"
#include "KG3DTextureTable.h"
#include "KG3DModel.h"
#include "KG3DMesh.h"
#include "KG3DSFX.h"

#pragma warning(disable : 4533)


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DSFXBillboard::KG3DSFXBillboard(KG3DSFX *pSFX)
{
    ASSERT(pSFX);

    m_pSFX = pSFX;

    m_pTexture = NULL;
    m_dwCuttingNum = 1;
    m_pTexture2 = NULL;
    m_dwCuttingNum2 = 1;

    m_fCurrentFrame = -1;

    //m_bLockAxis = FALSE;
    //m_bRotation = FALSE;

    m_bBindToBone = FALSE;
    m_strBoneName = "";

    m_vPositionLine.InsertKeyFrame(0, D3DXVECTOR3(0, 0, 0));
    m_fWidthTimeLine.InsertKeyFrame(0, 50);
    m_fHeightTimeLine.InsertKeyFrame(0, 50);
    m_DiffuseTimeLine.InsertKeyFrame(0, D3DXCOLOR(1, 1, 1, 1));
    m_nTextureFrameIndex.InsertKeyFrame(0, 0);
    m_nTexture2FrameIndex.InsertKeyFrame(0, 0);
    m_vUVTimeLine.InsertKeyFrame(0, D3DXVECTOR2(0, 0));
    m_fRotationTimeLine.InsertKeyFrame(0, 0.f);

    m_dwBindTrackIndex = SFX_NOT_BIND_TO_TRACK;
    m_Rotation = 0.f;
    m_Translation = D3DXVECTOR3(0, 0, 0);

    m_eBlendMode = SFX_BM_SRCALPHA_INVSRCALPHA;
    m_dwRenderTargetTexFlag = 0;
    m_dwBillBoradType = BILLBOARE_TYPE_NORMAL;
    m_pVB = NULL;

    m_fHeight = 0.f;
    m_fWidth  = 0.f;
}

KG3DSFXBillboard::~KG3DSFXBillboard(void)
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pTexture);
    SAFE_RELEASE(m_pTexture2);
}

HRESULT KG3DSFXBillboard::OnLostDevice()
{
    SAFE_RELEASE(m_pVB);
    return S_OK;
}

HRESULT KG3DSFXBillboard::OnResetDevice()
{
    return Init();
}


HRESULT KG3DSFXBillboard::CreateVertexBuffer()
{
	HRESULT hRetCode = S_OK;

	if(!m_pVB)
	{
		DWORD dwThreadID = GetCurrentThreadId();
		ASSERT(dwThreadID == g_dwMainThreadID);

		hRetCode = g_pd3dDevice->CreateVertexBuffer(
			4 * sizeof(VFormat::_Faces_Diffuse_Texture2),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
			D3DFVF_Faces_Diffuse_Texture2,
			D3DPOOL_DEFAULT,
			&m_pVB,
			NULL 
			);
	}

	return hRetCode;
}

HRESULT KG3DSFXBillboard::Init()
{
   return S_OK;
}

void KG3DSFXBillboard::GetAABBox(AABBOX *pBox)
{
    ASSERT(m_pSFX);
    m_AABBox.Clear();

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();

    D3DXVECTOR3 r = D3DXVECTOR3(-1.f, 0.f, 0.f);
    D3DXVECTOR3 u = D3DXVECTOR3( 0.f, 1.f, 0.f);

    if (pCamera)
    {
        r = pCamera->GetCameraRight();
        u = pCamera->GetCameraUp();
    }

    float fCurFrame = m_pSFX->GetCurrentFrame();

    float fWidth  = 10.f;
    float fHeight = 10.f;

    m_fWidthTimeLine.GetData(&fWidth, fCurFrame);
    m_fHeightTimeLine.GetData(&fHeight, fCurFrame);

    fWidth  =  fWidth  / 2.f + 2;
    fHeight =  fHeight / 2.f + 2;

    D3DXVECTOR3 s;
    
    s = m_Translation - u * fHeight - r * fWidth;
    m_AABBox.AddPosition(s);

    s = m_Translation - u * fHeight + r * fWidth;
    m_AABBox.AddPosition(s);

    s = m_Translation + u * fHeight - r * fWidth;
    m_AABBox.AddPosition(s);

    s = m_Translation + u * fHeight + r * fWidth;
    m_AABBox.AddPosition(s);

    *pBox = m_AABBox;

}

SFX_BLEND_MODE KG3DSFXBillboard::GetBlendMode()
{
    return m_eBlendMode;
}

void KG3DSFXBillboard::SetBlendMode(SFX_BLEND_MODE bm)
{
    m_eBlendMode = bm;
}


void KG3DSFXBillboard::SetRotation(float* pValue)
{
    if (!pValue)
        return;

    m_Rotation = *pValue;
}

void KG3DSFXBillboard::GetRotation(float* pValue)
{
    if (!pValue)
        return;

    *pValue = m_Rotation;
}

void KG3DSFXBillboard::SetTranslation(D3DXVECTOR3 *pValue)
{
    if (!pValue)
        return;

    m_Translation = *pValue;
}

void KG3DSFXBillboard::GetTranslation(D3DXVECTOR3 *pValue)
{
    if (!pValue)
        return;

    *pValue = m_Translation;
}

void  KG3DSFXBillboard::InsertRtsKeyFrame(int nFrame, DWORD dwFlag)
{
    ASSERT(nFrame >= 0);

    D3DXVECTOR3 t;
    float w;
    float h;

    if (dwFlag & SET_TRANS) {
        m_vPositionLine.InsertKeyFrame(nFrame, m_Translation);
    } else if (SUCCEEDED(m_vPositionLine.GetPrevFrameData(&t, nFrame))) {
        if (t != m_Translation) {
            m_vPositionLine.InsertKeyFrame(nFrame, m_Translation);
        }
    } else {
        m_vPositionLine.InsertKeyFrame(nFrame, m_Translation);
    }

    if (dwFlag & SET_SCALG) {
        m_fWidthTimeLine.InsertKeyFrame(nFrame, m_fWidth);
    } else if (SUCCEEDED(m_fWidthTimeLine.GetPrevFrameData(&w, nFrame))) {
        if (w != m_fWidth) {
            m_fWidthTimeLine.InsertKeyFrame(nFrame, m_fWidth);
        }
    } else {
        m_fWidthTimeLine.InsertKeyFrame(nFrame, m_fWidth);
    }

    if (dwFlag & SET_SCALG) {
        m_fHeightTimeLine.InsertKeyFrame(nFrame, m_fHeight);
    } else if (SUCCEEDED(m_fHeightTimeLine.GetPrevFrameData(&h, nFrame))) {
        if (h != m_fHeight) {
            m_fHeightTimeLine.InsertKeyFrame(nFrame, m_fHeight);
        }
    } else {
        m_fHeightTimeLine.InsertKeyFrame(nFrame, m_fHeight);
    }
}

HRESULT KG3DSFXBillboard::Render(float fCurrentFrame)
{
    HRESULT hRetCode = E_FAIL;

	CreateVertexBuffer();

    if (!m_pVB)
        return E_FAIL;

	BOOL bTextureReady = FALSE;

    if (m_fCurrentFrame != fCurrentFrame)
    {
        /*
         * update the rtx data
         */

        m_vPositionLine.GetData(&m_Translation, fCurrentFrame);
        m_fRotationTimeLine.GetData(&m_Rotation, fCurrentFrame);
        m_fWidthTimeLine.GetData(&m_fWidth, fCurrentFrame);
        m_fHeightTimeLine.GetData(&m_fHeight, fCurrentFrame);
        m_DiffuseTimeLine.GetData(&m_Color, fCurrentFrame);

        m_fCurrentFrame = fCurrentFrame;
    }


    /*
     * update to world matrix
     */

    D3DXMatrixTranslation(&m_Matrix, m_Translation.x, m_Translation.y, m_Translation.z);
    if (m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
    {
        KG3DSFXTrack *pTrack = NULL;
        D3DXMATRIX matTrack;
        if (SUCCEEDED(m_pSFX->GetTrack(m_dwBindTrackIndex, &pTrack)))
        {
            pTrack->GetTransform(fCurrentFrame, &matTrack);
            m_Matrix *= matTrack;
        }
    }

    m_matWorld = m_Matrix * (*m_pSFX->GetCurrentWorldMatrix());

    /*
     * crate billboard 
     */

    D3DXVECTOR3 vPos = D3DXVECTOR3(m_matWorld._41, m_matWorld._42, m_matWorld._43);
    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    ASSERT(pCamera);

    float fHScal =  m_pSFX->m_Srt.vScanl.y;
    float fWScal = (m_pSFX->m_Srt.vScanl.x + m_pSFX->m_Srt.vScanl.z) * 0.5f;

    float HalfWidth  = m_fWidth * fWScal * 0.5f;
    float HalfHeight = m_fHeight * fHScal *  0.5f;

    D3DXVECTOR3 vRight;
    D3DXVECTOR3 vUp;

    switch (m_dwBillBoradType)
    {
    case BILLBOARE_TYPE_NORMAL :
        {
            vRight = pCamera->GetCameraRight();
            vUp = pCamera->GetCameraUp();
        }
        break;
    case BILLBOARE_TYPE_FREE :
        {
            ASSERT(false);
        }
        break;
    case BILLBOARE_TYPE_LOCK_X :
        {
            vRight = pCamera->GetCameraRight();
            vUp = pCamera->GetCameraFront();
            vUp.y = 0.f;
            D3DXVec3Normalize(&vUp, &vUp);
        }
        break;
    case BILLBOARE_TYPE_LOCK_Y :
        {
            vUp = D3DXVECTOR3(0.f, 1.f, 0.f);
            D3DXVECTOR3 vCameraPos;
            pCamera->GetPosition(&vCameraPos);
            D3DXVECTOR3 vLookDirection = vCameraPos - vPos;
            D3DXVec3Cross(&vRight, &vLookDirection, &vUp);
            D3DXVec3Normalize(&vRight, &vRight);
        }
        break;
    default :
        ASSERT(false);
        break;
    }


    if (m_fRotationTimeLine.GetSize() > 1)
    {
        D3DXVECTOR3 vAxis;
        D3DXMATRIX mat;
        D3DXVec3Cross(&vAxis, &vUp, &vRight);
        D3DXVec3Normalize(&vAxis, &vAxis);
        D3DXMatrixRotationAxis(&mat, &vAxis, m_Rotation);
        D3DXVec3TransformNormal(&vRight, &vRight, &mat);
        D3DXVec3TransformNormal(&vUp, &vUp, &mat);
    }

    VFormat::_Faces_Diffuse_Texture2* pVertex = NULL;
    hRetCode = m_pVB->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD);
    if (FAILED(hRetCode))
        return hRetCode;

    pVertex[0].p = vPos - vRight * HalfWidth + vUp * HalfHeight;
    pVertex[1].p = vPos + vRight * HalfWidth + vUp * HalfHeight;
    pVertex[2].p = vPos - vRight * HalfWidth - vUp * HalfHeight;
    pVertex[3].p = vPos + vRight * HalfWidth - vUp * HalfHeight;
    pVertex[0].diffuse = (D3DCOLOR)m_Color;
    pVertex[1].diffuse = (D3DCOLOR)m_Color;
    pVertex[2].diffuse = (D3DCOLOR)m_Color;
    pVertex[3].diffuse = (D3DCOLOR)m_Color;

    if (m_pTexture && m_pTexture->GetTexture())
    {
        int nTextureFrameIndex = 0;
        m_nTextureFrameIndex.GetData(&nTextureFrameIndex, fCurrentFrame);
        if (nTextureFrameIndex >= (int)(m_dwCuttingNum * m_dwCuttingNum))
            nTextureFrameIndex = m_dwCuttingNum * m_dwCuttingNum - 1;

        D3DXVECTOR2 vUV1 = D3DXVECTOR2(0.f, 0.f);
        if (m_dwCuttingNum == 1 && m_dwCuttingNum2 == 1)
            m_vUVTimeLine.GetData(&vUV1, fCurrentFrame);
        else
        {
            vUV1.x = 0;
            vUV1.y = 0;
        }

        float fCellSize1 = 1.0f / (float)m_dwCuttingNum;
        UINT uY1 = nTextureFrameIndex / m_dwCuttingNum;
        UINT uX1 = nTextureFrameIndex - uY1 * m_dwCuttingNum;

        pVertex[0].tu1 = uX1 * fCellSize1 + vUV1.x;
        pVertex[0].tv1 = uY1 * fCellSize1 + vUV1.y;

        pVertex[1].tu1 = (uX1 + 1) * fCellSize1 + vUV1.x;
        pVertex[1].tv1 = uY1 * fCellSize1 + vUV1.y;

        pVertex[2].tu1 = uX1 * fCellSize1 + vUV1.x;
        pVertex[2].tv1 = (uY1 + 1) * fCellSize1 + vUV1.y;

        pVertex[3].tu1 = (uX1 + 1) * fCellSize1 + vUV1.x;
        pVertex[3].tv1 = (uY1 + 1) * fCellSize1 + vUV1.y;

        g_pd3dDevice->SetTexture(0, m_pTexture->GetTexture());

        if (m_pTexture2 && m_pTexture2->GetTexture())
        {
            int nTexture2FrameIndex = 0;
            m_nTexture2FrameIndex.GetData(&nTexture2FrameIndex, fCurrentFrame);
            if (nTexture2FrameIndex >= (int)(m_dwCuttingNum2 * m_dwCuttingNum2))
                nTexture2FrameIndex = m_dwCuttingNum2 * m_dwCuttingNum2 - 1;

            float fCellSize2 = 1.0f / (float)m_dwCuttingNum2;
            UINT uY2 = nTexture2FrameIndex / m_dwCuttingNum2;
            UINT uX2 = nTexture2FrameIndex - uY2 * m_dwCuttingNum2;

            pVertex[0].tu2 = uX2 * fCellSize2;
            pVertex[0].tv2 = uY2 * fCellSize2;

            pVertex[1].tu2 = (uX2 + 1) * fCellSize2;
            pVertex[1].tv2 = uY2 * fCellSize2;

            pVertex[2].tu2 = uX2 * fCellSize2;
            pVertex[2].tv2 = (uY2 + 1) * fCellSize2;

            pVertex[3].tu2 = (uX2 + 1) * fCellSize2;
            pVertex[3].tv2 = (uY2 + 1) * fCellSize2;
        }

		bTextureReady = TRUE;
    }

    m_pVB->Unlock();


    if (m_pTexture && m_pTexture->GetTexture())
    {
        g_pd3dDevice->SetTexture(0, m_pTexture->GetTexture());
        if (m_pTexture2 && m_pTexture2->GetTexture())
            g_pd3dDevice->SetTexture(1, m_pTexture2->GetTexture());
        else
            g_pd3dDevice->SetTexture(1, NULL);

		bTextureReady = TRUE;
    }
    else 
        g_pd3dDevice->SetTexture(0, NULL);

	if(bTextureReady)
	{
		D3DXMATRIX matWorldSave;
		D3DXMATRIX matRender;
		D3DXMatrixIdentity(&matRender);
		hRetCode = g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
		hRetCode = g_pd3dDevice->SetTransform(D3DTS_WORLD, &matRender);
		hRetCode = g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(VFormat::_Faces_Diffuse_Texture2));
		hRetCode = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		hRetCode = g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
		hRetCode = g_pd3dDevice->SetStreamSource(0, 0, 0, 0);
	}


    return S_OK;

    /*
    HRESULT hResult  = E_FAIL;

    if (m_fCurrentFrame != fCurrentFrame)
    {
        m_vPositionLine.GetData(&m_Translation, fCurrentFrame);
        m_fCurrentFrame = fCurrentFrame;
    }

    D3DXMATRIX     mat;
    D3DXQUATERNION q;

    float fHScal =  m_pSFX->m_Srt.vScanl.y;
    float fWScal = (m_pSFX->m_Srt.vScanl.x + m_pSFX->m_Srt.vScanl.z) * 0.5f;

    D3DXQuaternionRotationYawPitchRoll(&q, m_Rotation.y, m_Rotation.x, m_Rotation.z);
    D3DXMatrixTransformation(
        &mat,
        &D3DXVECTOR3(0, 0, 0),
        &D3DXQUATERNION(0, 0, 0, 1),
        &D3DXVECTOR3(1, 1, 1),
        &D3DXVECTOR3(0, 0, 0),
        &q,
        &m_Translation
        );
    if (m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
    {
        D3DXMATRIX matTrack;
        KG3DSFXTrack *pTrack = NULL;
        if (SUCCEEDED(m_pSFX->GetTrack(m_dwBindTrackIndex, &pTrack)))
        {
            pTrack->GetTransform(fCurrentFrame, &matTrack);
            mat *= matTrack;
        }
    }
    m_Matrix = mat;

    D3DXVECTOR3 vPos(mat._41, mat._42, mat._43);
    D3DXVec3TransformCoord(&vPos, &vPos, m_pSFX->GetCurrentWorldMatrix());

    float fWidth  = 1.f;
    float fHeight = 1.f;
    
    m_fWidthTimeLine.GetData(&fWidth, fCurrentFrame);
    m_fHeightTimeLine.GetData(&fHeight, fCurrentFrame);

    fWidth  *= fWScal;//m_pSFX->GetScaleSize();
    fHeight *= fHScal;//m_pSFX->GetScaleSize();

    D3DXCOLOR color;
    m_DiffuseTimeLine.GetData(&color, fCurrentFrame);

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    ASSERT(pCamera);

    float HalfWidth = fWidth * 0.5f;
    float HalfHeight = fHeight * 0.5f;

    D3DXVECTOR3 vRight;
    D3DXVECTOR3 vUp;

    if (m_bRotation && m_fRotationTimeLine.GetSize())
    {
        ASSERT(!m_bLockAxis);

        vRight = pCamera->GetCameraRight();
        vUp = pCamera->GetCameraUp();

        D3DXVECTOR3 vAxis = pCamera->GetCameraFront();
        D3DXVec3Normalize(&vAxis, &vAxis);
        D3DXMATRIX mat;
        float fAngle = 0;
        m_fRotationTimeLine.GetData(&fAngle, fCurrentFrame);
        D3DXMatrixRotationAxis(&mat, &vAxis, fAngle);
        D3DXVec3TransformCoord(&vRight, &vRight, &mat);
        D3DXVec3TransformCoord(&vUp, &vUp, &mat);
    }
    else if (m_bLockAxis)
    {
        ASSERT(!m_bRotation);

        vUp.x = mat._21;
        vUp.y = mat._22;
        vUp.z = mat._23;

        D3DXVECTOR3 vCameraPos;
        pCamera->GetPosition(&vCameraPos);

        D3DXVECTOR3 vLookDirection = vCameraPos - vPos;
        D3DXVec3Cross(&vRight, &vLookDirection, &vUp);
    }
    else
    {
        vRight = pCamera->GetCameraRight();
        vUp = pCamera->GetCameraUp();
    }

    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    VFormat::_Faces_Diffuse_Texture2 vertices[4];

    vertices[0].p = vPos - vRight * HalfWidth + vUp * HalfHeight;
    vertices[1].p = vPos + vRight * HalfWidth + vUp * HalfHeight;
    vertices[2].p = vPos - vRight * HalfWidth - vUp * HalfHeight;
    vertices[3].p = vPos + vRight * HalfWidth - vUp * HalfHeight;

    vertices[0].diffuse = (D3DCOLOR)color;
    vertices[1].diffuse = (D3DCOLOR)color;
    vertices[2].diffuse = (D3DCOLOR)color;
    vertices[3].diffuse = (D3DCOLOR)color;

    if (m_pTexture && m_pTexture->GetTexture())
    {
        int nTextureFrameIndex = 0;
        m_nTextureFrameIndex.GetData(&nTextureFrameIndex, fCurrentFrame);
        if (nTextureFrameIndex >= (int)(m_dwCuttingNum * m_dwCuttingNum))
            nTextureFrameIndex = m_dwCuttingNum * m_dwCuttingNum - 1;

        D3DXVECTOR2 vUV1 = D3DXVECTOR2(0.f, 0.f);
        if (m_dwCuttingNum == 1 && m_dwCuttingNum2 == 1)
            m_vUVTimeLine.GetData(&vUV1, fCurrentFrame);
        else
        {
            vUV1.x = 0;
            vUV1.y = 0;
        }

        float fCellSize1 = 1.0f / (float)m_dwCuttingNum;
        UINT uY1 = nTextureFrameIndex / m_dwCuttingNum;
        UINT uX1 = nTextureFrameIndex - uY1 * m_dwCuttingNum;

        vertices[0].tu1 = uX1 * fCellSize1 + vUV1.x;
        vertices[0].tv1 = uY1 * fCellSize1 + vUV1.y;

        vertices[1].tu1 = (uX1 + 1) * fCellSize1 + vUV1.x;
        vertices[1].tv1 = uY1 * fCellSize1 + vUV1.y;

        vertices[2].tu1 = uX1 * fCellSize1 + vUV1.x;
        vertices[2].tv1 = (uY1 + 1) * fCellSize1 + vUV1.y;

        vertices[3].tu1 = (uX1 + 1) * fCellSize1 + vUV1.x;
        vertices[3].tv1 = (uY1 + 1) * fCellSize1 + vUV1.y;

        g_pd3dDevice->SetTexture(0, m_pTexture->GetTexture());

        if (m_pTexture2 && m_pTexture2->GetTexture())
        {
            int nTexture2FrameIndex = 0;
            m_nTexture2FrameIndex.GetData(&nTexture2FrameIndex, fCurrentFrame);
            if (nTexture2FrameIndex >= (int)(m_dwCuttingNum2 * m_dwCuttingNum2))
                nTexture2FrameIndex = m_dwCuttingNum2 * m_dwCuttingNum2 - 1;

            float fCellSize2 = 1.0f / (float)m_dwCuttingNum2;
            UINT uY2 = nTexture2FrameIndex / m_dwCuttingNum2;
            UINT uX2 = nTexture2FrameIndex - uY2 * m_dwCuttingNum2;

            vertices[0].tu2 = uX2 * fCellSize2;
            vertices[0].tv2 = uY2 * fCellSize2;

            vertices[1].tu2 = (uX2 + 1) * fCellSize2;
            vertices[1].tv2 = uY2 * fCellSize2;

            vertices[2].tu2 = uX2 * fCellSize2;
            vertices[2].tv2 = (uY2 + 1) * fCellSize2;

            vertices[3].tu2 = (uX2 + 1) * fCellSize2;
            vertices[3].tv2 = (uY2 + 1) * fCellSize2;

            g_pd3dDevice->SetTexture(1, m_pTexture2->GetTexture());
        }
        else
        {
            g_pd3dDevice->SetTexture(1, NULL);
        }
    }
    else
        g_pd3dDevice->SetTexture(0, NULL);
 
    g_pd3dDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLESTRIP, 2, (void*)vertices, sizeof(VFormat::_Faces_Diffuse_Texture2)
        );

    // update bbox

   /* m_AABBox.Clear();
    m_AABBox.AddPosition(vertices[0].p);
    m_AABBox.AddPosition(vertices[1].p);
    m_AABBox.AddPosition(vertices[2].p);
    m_AABBox.AddPosition(vertices[3].p);*/

 //   hResult = S_OK;
//Exit0:
   // return hResult;

}

//BOOL KG3DSFXBillboard::IsRotation()
//{
//    return m_bRotation;
//}
//
//BOOL KG3DSFXBillboard::IsLockAxis()
//{
//    return m_bLockAxis;
//}

HRESULT KG3DSFXBillboard::RenderSelectMark()
{
	D3DXVECTOR3 r;
	D3DXVECTOR3 u;

	KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
	ASSERT(pCamera);
    ASSERT(m_pSFX);

	r = pCamera->GetCameraRight();
	u = pCamera->GetCameraUp();

    float fCurFrame = m_pSFX->GetCurrentFrame();

    float fWidth  = 10.f;
    float fHeight = 10.f;

    m_fWidthTimeLine.GetData(&fWidth, fCurFrame);
    m_fHeightTimeLine.GetData(&fHeight, fCurFrame);

    fWidth  =  fWidth  / 2.f + 2;
    fHeight =  fHeight / 2.f + 2;

    D3DXVECTOR3 s;
    D3DXVECTOR3 d;

    s = m_Translation - u * fHeight - r * fWidth;
    d = m_Translation - u * fHeight + r * fWidth;

    g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);

    s = m_Translation - u * fHeight + r * fWidth;
    d = m_Translation + u * fHeight + r * fWidth;

    g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);

    s = m_Translation + u * fHeight + r * fWidth;
    d = m_Translation + u * fHeight - r * fWidth;

    g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);

    s = m_Translation + u * fHeight - r * fWidth;
    d = m_Translation - u * fHeight - r * fWidth;

    g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);



	/*D3DXVECTOR3 s;
	D3DXVECTOR3 d;

	s = m_Translation - r * 10.f;
	d = m_Translation + r * 10.f;

	g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);

	s = m_Translation - u * 10.f;
	d = m_Translation + u * 10.f;

	g_cGraphicsTool.DrawLine(&s, &d, 0xFFFF0000, 0xFFFF0000);*/

    return S_OK;
}


HRESULT KG3DSFXBillboard::LoadTexture(const char szTextureName[])
{
    KG3DTexture *pTexture = NULL;
    HRESULT hr;
    if (szTextureName == NULL || szTextureName[0] == '\0')
        hr = S_OK;
    else
	{	
		/*
        if (m_dwRenderTargetTexFlag && 0)
            hr = g_cTextureTable.LoadResourceFromFile(szTextureName, 0, TEXOP_RENDERTARGET | TEXOP_DISABLEPOW2 | TEXOP_DISABLEMIPMAP,
            (IKG3DResourceBase **)&pTexture);
        else */
            hr = g_cTextureTable.LoadResourceFromFile(szTextureName, 0, TEXOP_DISABLEDTX/*|TEXOP_RELOAD*/,
				(IKG3DResourceBase **)&pTexture);
	}

    if (SUCCEEDED(hr))
    {
        SAFE_RELEASE(m_pTexture);
        m_pTexture = pTexture;
    }
    else
    {
        KGLogPrintf(KGLOG_DEBUG, TEXT("can not open texture : %s, %s"), szTextureName, KG_FUNCTION);
    }

    return hr;
}

HRESULT KG3DSFXBillboard::SetTextureCutNum(DWORD dwCuttingNum)
{
    m_dwCuttingNum = dwCuttingNum;
    return S_OK;
}

HRESULT KG3DSFXBillboard::LoadTexture2(const char szTextureName[])
{
    KG3DTexture *pTexture = NULL;
    HRESULT hr;
    if (szTextureName == NULL || szTextureName[0] == '\0')
        hr = S_OK;
    else
    {	
		/*
        if (m_dwRenderTargetTexFlag && 0)
            hr = g_cTextureTable.LoadResourceFromFile(szTextureName, 0, TEXOP_RENDERTARGET | TEXOP_DISABLEPOW2 | TEXOP_DISABLEMIPMAP,
            (IKG3DResourceBase **)&pTexture);
        else */
            hr = g_cTextureTable.LoadResourceFromFile(szTextureName, 0, TEXOP_DISABLEDTX/*|TEXOP_RELOAD*/,
            (IKG3DResourceBase **)&pTexture);
    }

    if (SUCCEEDED(hr))
    {
        SAFE_RELEASE(m_pTexture2);
        m_pTexture2 = pTexture;
    }

    return hr;
}

HRESULT KG3DSFXBillboard::SetTexture2CutNum(DWORD dwCuttingNum)
{
    m_dwCuttingNum2 = dwCuttingNum;
    return S_OK;
}

DWORD KG3DSFXBillboard::GetTexture2CutNum()
{
    return m_dwCuttingNum2;
}

DWORD KG3DSFXBillboard::GetTextureCutNum()
{
    return m_dwCuttingNum;
}

//HRESULT KG3DSFXBillboard::EnableLockAxis(BOOL bEnable)
//{
//    m_bLockAxis = bEnable;
//    if (bEnable)
//        m_bRotation = FALSE;
//
//    return S_OK;
//}
//
//HRESULT KG3DSFXBillboard::EnableRotation(BOOL bEnable)
//{
//    m_bRotation = bEnable;
//    if (bEnable)
//        m_bLockAxis = FALSE;
//
//    return S_OK;
//}

IEKG3DTexture* KG3DSFXBillboard::GetTexture()
{
    return m_pTexture;
}

IEKG3DTexture* KG3DSFXBillboard::GetTexture2()
{
    return m_pTexture2;
}

HRESULT KG3DSFXBillboard::BindToBone(const char cszBoneName[])
{
    m_bBindToBone = TRUE;
    m_strBoneName = cszBoneName;
    return S_OK;
}

HRESULT KG3DSFXBillboard::UnBind()
{
    m_bBindToBone = FALSE;
    m_strBoneName = "";
    return S_OK;
}

BOOL KG3DSFXBillboard::IsBindToBone() const
{
    return m_bBindToBone;
}

std::string KG3DSFXBillboard::GetBindName() const
{
    return m_strBoneName;
}

HRESULT KG3DSFXBillboard::Clone(KG3DSFXBillboard **ppBillboard)
{
    ASSERT(ppBillboard);
    
    if (FAILED(m_pSFX->AddBillboard(ppBillboard)))
        return E_FAIL;

    (*ppBillboard)->m_vPositionLine = m_vPositionLine;
    (*ppBillboard)->m_fWidthTimeLine = m_fWidthTimeLine;
    (*ppBillboard)->m_fHeightTimeLine = m_fHeightTimeLine;
    (*ppBillboard)->m_DiffuseTimeLine = m_DiffuseTimeLine;
    (*ppBillboard)->m_nTextureFrameIndex = m_nTextureFrameIndex;
    (*ppBillboard)->m_nTexture2FrameIndex = m_nTexture2FrameIndex;
    (*ppBillboard)->m_vUVTimeLine = m_vUVTimeLine;
    (*ppBillboard)->m_fRotationTimeLine = m_fRotationTimeLine;

    (*ppBillboard)->m_pTexture = m_pTexture;
    if (m_pTexture)
        m_pTexture->AddRef();

    (*ppBillboard)->m_pTexture2 = m_pTexture2;
    if (m_pTexture2)
        m_pTexture2->AddRef();
    
    (*ppBillboard)->m_dwCuttingNum = m_dwCuttingNum;
    (*ppBillboard)->m_dwCuttingNum2 = m_dwCuttingNum2;
    (*ppBillboard)->m_Translation = m_Translation;
    (*ppBillboard)->m_Rotation = m_Rotation;
    (*ppBillboard)->m_dwBillBoradType = m_dwBillBoradType;
    //(*ppBillboard)->m_bRotation = m_bRotation;
    (*ppBillboard)->m_fCurrentFrame = m_fCurrentFrame;
    (*ppBillboard)->m_eBlendMode = m_eBlendMode;
    (*ppBillboard)->m_fHeight = m_fHeight;
    (*ppBillboard)->m_fWidth = m_fWidth;

    return S_OK;
}

HRESULT KG3DSFXBillboard::BindToTrack(DWORD dwIndex)
{
    if (dwIndex < m_pSFX->GetTrackNum())
    {
        m_dwBindTrackIndex = dwIndex;
        return S_OK;
    }

    return E_FAIL;
}

HRESULT KG3DSFXBillboard::UnBindToTrack()
{
    m_dwBindTrackIndex = SFX_NOT_BIND_TO_TRACK;
    return S_OK;
}

BOOL KG3DSFXBillboard::IsBindToTrack() const
{
    return m_dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK;
}

DWORD KG3DSFXBillboard::GetBindTrackIndex() const
{
    return m_dwBindTrackIndex;
}


/*这部分工作交给Entity完成，这个类只管处理一个SetRotation就好 Last modify by Chen Tianhong: 2007-8-16 10:44:20
HRESULT KG3DSFXBillboard::RotationY(FLOAT fAngelY)
{
    D3DXQUATERNION q;
    m_Rotation.y += fAngelY;
    return S_OK;
}


HRESULT KG3DSFXBillboard::RotationZ(FLOAT fAngelZ)
{
    m_Rotation.z += fAngelZ;
    return S_OK;
}


HRESULT KG3DSFXBillboard::RotationX(FLOAT fAngelX)
{
    D3DXQUATERNION q;
    m_Rotation.x += fAngelX;
    return S_OK;
}*/


