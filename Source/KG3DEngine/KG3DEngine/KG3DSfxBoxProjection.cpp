
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSfxBoxProjection.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2009-50-12 16:11:41
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSfxBoxProjection.h"
#include "KG3DScene.h"
#include "KG3DModel.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSfx.h"
#include "KG3DBsp.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"

KG3DSfxBoxProjection::KG3DSfxBoxProjection(KG3DSFX* pParentSfx)
    : m_pSfx(pParentSfx),
      m_pVB(NULL),
      m_pSysVB(NULL),
      m_nMaxFaceNum(2048),
      m_nFaceNum(0),
      m_pCurrentOpModel(NULL),
      m_pTexture(NULL),
      m_dwProjType(BOX_PROJ_Y),
      m_eBlend(SFX_BM_SRCALPHA_INVSRCALPHA),
      m_nClipU(1),
      m_nClipV(1),
      m_fUVFrameSpeed(0.f)
{
    m_tlColor.InsertKeyFrame(0, 0xffffffff);
    m_tlUV.InsertKeyFrame(0, D3DXVECTOR4(0.f, 0.f, 1.f, 1.f));
}

KG3DSfxBoxProjection::~KG3DSfxBoxProjection()
{
    Clear();
}

namespace {

    bool OnFindFacs(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& boxHalfSize, void* param)
    {
        if (!param)
            return false;

        KG3DSfxBoxProjection* pBoxProj = (KG3DSfxBoxProjection*)param;
        ASSERT(pBoxProj->m_nFaceNum < pBoxProj->m_nMaxFaceNum);

        int nStartIndex = pBoxProj->m_nFaceNum * 3;

        pBoxProj->m_pSysVB[nStartIndex].p = v0;
        ++nStartIndex;

        pBoxProj->m_pSysVB[nStartIndex].p = v1;
        ++nStartIndex;

        pBoxProj->m_pSysVB[nStartIndex].p = v2;
        ++nStartIndex;

        ++pBoxProj->m_nFaceNum;

        if (pBoxProj->m_nFaceNum >= pBoxProj->m_nMaxFaceNum)
            return false;

        return true;
    }

    __forceinline float hundredlize(float fValue, float& fOffset)
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
}

D3DXVECTOR3 KG3DSfxBoxProjection::m_sBox[8] = 
{
    D3DXVECTOR3(-Local_Box_Size,  Local_Box_Size,  Local_Box_Size),
    D3DXVECTOR3( Local_Box_Size,  Local_Box_Size,  Local_Box_Size),
    D3DXVECTOR3( Local_Box_Size,  Local_Box_Size, -Local_Box_Size),
    D3DXVECTOR3(-Local_Box_Size,  Local_Box_Size, -Local_Box_Size),
    D3DXVECTOR3(-Local_Box_Size, -Local_Box_Size,  Local_Box_Size),
    D3DXVECTOR3( Local_Box_Size, -Local_Box_Size,  Local_Box_Size),
    D3DXVECTOR3( Local_Box_Size, -Local_Box_Size, -Local_Box_Size),
    D3DXVECTOR3(-Local_Box_Size, -Local_Box_Size, -Local_Box_Size)
};

HRESULT KG3DSfxBoxProjection::Create()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    const int    maxProjNum = 32;
    KG3DScene*   pScene = g_cGraphicsTool.GetCurScene();
    KG3DModel*   pModels[maxProjNum];
    int          nProjNum = 0;
    _Vertex*     pVertex = NULL;
    D3DXVECTOR3  vRectXZ[4];
    D3DXVECTOR3  vStart;
    D3DXVECTOR3  vEnd;
    D3DXVECTOR3  vCurr;
    D3DXVECTOR3  box[8];
    float        fOffset = 0.f;
    D3DXMATRIX   matBox;
    D3DXVECTOR3  hs = D3DXVECTOR3(Local_Box_Size, Local_Box_Size, Local_Box_Size);

    KG_PROCESS_ERROR(m_pSfx);
    KG_PROCESS_ERROR(pScene);

    Clear();

    D3DXVec3TransformCoordArray(box, sizeof(D3DXVECTOR3), m_sBox, sizeof(D3DXVECTOR3), &m_matWorld, 8);
    D3DXMatrixInverse(&matBox, NULL, &m_matWorld);    

    nProjNum = pScene->GetModelOverlapBox(pModels, maxProjNum, box);

    ASSERT(!m_pSysVB);
    m_pSysVB = new _Vertex[m_nMaxFaceNum * 3];

    m_BBox.Clear();
    m_BBox.AddPosition(box, 8);

    vStart = D3DXVECTOR3(hundredlize(m_BBox.A.x, fOffset) - 100.f, m_BBox.A.y, hundredlize(m_BBox.A.z, fOffset) - 100.f);
    vEnd   = D3DXVECTOR3(hundredlize(m_BBox.B.x, fOffset) + 100.f, m_BBox.B.y, hundredlize(m_BBox.B.z, fOffset) + 100.f);
    vCurr  = vStart;

    if (vCurr.x < vEnd.x && vCurr.z < vEnd.z)
    {      
        while (vCurr.x < vEnd.x && vCurr.z < vEnd.z)
        {
            D3DXVECTOR3 tri_1[] = {
                D3DXVECTOR3(vCurr.x, vCurr.y, vCurr.z),
                D3DXVECTOR3(vCurr.x, vCurr.y, vCurr.z + 100.f),
                D3DXVECTOR3(vCurr.x + 100.f, vCurr.y, vCurr.z)
            };

            D3DXVECTOR3 tri_2[] = {
                D3DXVECTOR3(vCurr.x, vCurr.y, vCurr.z + 100.f),
                D3DXVECTOR3(vCurr.x + 100.f, vCurr.y, vCurr.z + 100.f),
                D3DXVECTOR3(vCurr.x + 100.f, vCurr.y, vCurr.z)
            };

            for (int i = 0; i < 3; ++i)
            {
                tri_1[i].y = pScene->GetHeight(tri_1[i].x, tri_1[i].z);
                tri_2[i].y = pScene->GetHeight(tri_2[i].x, tri_2[i].z);
            }

            int nStartIndex = m_nFaceNum * 3;

            D3DXVec3TransformCoordArray(tri_1, sizeof(D3DXVECTOR3), tri_1, sizeof(D3DXVECTOR3), &matBox, 3);
            D3DXVec3TransformCoordArray(tri_2, sizeof(D3DXVECTOR3), tri_2, sizeof(D3DXVECTOR3), &matBox, 3);

            if (IsTriangleBoxOverlap(D3DXVECTOR3(0.f, 0.f, 0.f), hs, tri_1))
            {
                m_pSysVB[nStartIndex].p = tri_1[0];
                ++nStartIndex;

                m_pSysVB[nStartIndex].p = tri_1[1];
                ++nStartIndex;

                m_pSysVB[nStartIndex].p = tri_1[2];
                ++nStartIndex;

                ++m_nFaceNum;
            }

            if (m_nFaceNum >= m_nMaxFaceNum)
                break;

            if (IsTriangleBoxOverlap(D3DXVECTOR3(0.f, 0.f, 0.f), hs, tri_2))
            {
                m_pSysVB[nStartIndex].p = tri_2[0];
                ++nStartIndex;

                m_pSysVB[nStartIndex].p = tri_2[1];
                ++nStartIndex;

                m_pSysVB[nStartIndex].p = tri_2[2];
                ++nStartIndex;

                ++m_nFaceNum;
            }
            
            if (m_nFaceNum >= m_nMaxFaceNum)
                break;

            if (vCurr.x < vEnd.x - 100.f)
                vCurr.x += 100.f;
            else
            {
                vCurr.x = vStart.x;
                vCurr.z += 100.f;
            }
        }
    }


    if (m_nFaceNum < m_nMaxFaceNum)
    {
        for (int i = 0; i < nProjNum; ++i)
        {
            D3DXMATRIX matBoxSpace;
            m_pCurrentOpModel = pModels[i];

            matBoxSpace = m_pCurrentOpModel->m_matWorld * matBox;
            m_pCurrentOpModel->GetBsp()->GetFacesIntersectBox(matBoxSpace, hs, OnFindFacs, this);
            m_pCurrentOpModel = NULL;

            if (m_nFaceNum >= m_nMaxFaceNum)
                break;
        }
    }

    KG_PROCESS_SUCCESS(m_nFaceNum <= 0);
    
	{
		DWORD dwThreadID = GetCurrentThreadId();
		ASSERT(dwThreadID == g_dwMainThreadID);

		hRetCode = g_pd3dDevice->CreateVertexBuffer(sizeof(_Vertex) * m_nFaceNum * 3, 0, _Vertex::FVF, D3DPOOL_MANAGED, &m_pVB, NULL);
		KG_COM_PROCESS_ERROR(hRetCode);

		hRetCode = m_pVB->Lock(0, D3DUSAGE_WRITEONLY, (void**)&pVertex, D3DLOCK_NOSYSLOCK);
		KG_COM_PROCESS_ERROR(hRetCode);

		memcpy(pVertex, m_pSysVB, sizeof(_Vertex) * m_nFaceNum * 3);

		m_pVB->Unlock();

	}


Exit1 :
    hResult = S_OK;
Exit0 :
    SAFE_DELETE_ARRAY(m_pSysVB);
    return hResult;
}

HRESULT KG3DSfxBoxProjection::Clear()
{
    SAFE_RELEASE(m_pVB);
    m_nFaceNum = 0;
    return S_OK;
}

HRESULT KG3DSfxBoxProjection::OnLostDevice()
{
    return S_OK;
}

HRESULT KG3DSfxBoxProjection::OnResetDevice()
{
    return S_OK;
}

HRESULT KG3DSfxBoxProjection::GetAABBox(AABBOX* pBox)
{
    if (!pBox)
        return E_FAIL;

    *pBox = m_BBox;

    return S_OK;
}

void KG3DSfxBoxProjection::SetTexture(char szFileName[])
{
    SAFE_RELEASE(m_pTexture);
    g_cTextureTable.LoadResourceFromFile(szFileName, 0, 0, (IKG3DResourceBase**)&m_pTexture);
}

void KG3DSfxBoxProjection::GetTexture(char szFileName[])
{
    if (m_pTexture)
    {
        strcpy(szFileName, m_pTexture->m_scName.c_str());
    }
}

HRESULT KG3DSfxBoxProjection::FrameMove()
{
    HRESULT hResult  = E_FAIL;

    D3DXMATRIX matWorld;

    KG_PROCESS_ERROR(m_pSfx);

    matWorld = m_Matrix * (*m_pSfx->GetCurrentWorldMatrix());

    if (matWorld != m_matWorld)
    {
        m_matWorld = matWorld;
        Create();
    }

    hResult = S_OK;
Exit0 :
    return hResult;
}

void KG3DSfxBoxProjection::SetRenderState(KG3DRenderState& R, SFX_BLEND_MODE eBlendMode)
{
    switch (eBlendMode)
    {
    case SFX_BM_SRCALPHA_INVSRCALPHA :
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        break;

    case SFX_BM_SRCALPHA_ONE :
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        break;

    case SFX_BM_SRCALPHA_INVSRCCOLOR :
        R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
        R.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        R.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        break;

    case SFX_BM_SRCALPHA_SCREEN :
        R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        R.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
        R.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
        R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR );
        R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE );
        break;

    default :
        ASSERT(false);
        break;
    }
}

HRESULT KG3DSfxBoxProjection::Render()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DRenderState RS;
    D3DXMATRIX matSave;
    LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_SFX_BOX_PROJ);
    D3DXMATRIX matView;
    D3DXMATRIX matProj;
    D3DXMATRIX matWorldViewProj;
    D3DXCOLOR  color;
    D3DXVECTOR4 vColor;
    D3DXVECTOR4 vUV;
    
    float fSlop = 1.f;
    float fBais = -0.0001f;
    UINT uPass = 0;

    KG_PROCESS_ERROR(m_pVB);
    KG_PROCESS_ERROR(pShader);

    hRetCode = pShader->SetTechnique(TEXT("tec0"));
    KG_COM_PROCESS_ERROR(hRetCode);

    m_tlColor.GetData(&color, m_pSfx->GetCurrentFrame());
    vColor = D3DXVECTOR4(color.r, color.g, color.b, color.a);
    pShader->SetVector(TEXT("g_Color"), &vColor);

    m_tlUV.GetData(&vUV, m_pSfx->GetCurrentFrame());
    pShader->SetVector(TEXT("g_UV"), &vUV);

    g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
    g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    matWorldViewProj = m_matWorld * matView * matProj;
    pShader->SetMatrix(TEXT("g_matWorldViewProj"), &matWorldViewProj);

    RS.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlop);
    RS.SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBais);
    RS.SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    RS.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    SetRenderState(RS, m_eBlend);
    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

    g_cGraphicsTool.DrawOBBox(m_sBox, 0xff0000ff);

    g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(_Vertex));
    g_pd3dDevice->SetFVF(_Vertex::FVF);
    if (m_pTexture)
        g_pd3dDevice->SetTexture(0, m_pTexture->GetTexture());
    hRetCode = pShader->Begin(&uPass, 0);
    KG_COM_PROCESS_ERROR(hRetCode);
    hRetCode = pShader->BeginPass(m_dwProjType);
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_nFaceNum);
    pShader->EndPass();
    pShader->End();

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matSave);

    hResult = S_OK;
Exit0 :
    return hResult;
}