
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSfxExp.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-07-21 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSfxExp.h"
#include "KG3DGraphicsTool.h"
#include "KG3DScene.h"
#include "KG3DEngineManager.h"

namespace {
    inline float GetRandom(float fMin, float fMax)
    {
        float fRandNum = (float)rand() / RAND_MAX;
        return (fMin + (fMax - fMin) * fRandNum);
    }

    inline D3DXVECTOR3 operator* (const D3DXVECTOR3& v1, const D3DXVECTOR3& v2) 
    {
        return D3DXVECTOR3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }
}

KG3DSfxExp::KG3DSfxExp()
    : m_fPassTime(0.f),
      m_pBindModel(NULL)
{
    m_tlTimeScal.InsertKeyFrame(0, 1.f);
    m_tlCameraSwing.InsertKeyFrame(0, D3DXVECTOR3(0.f, 0.f, 0.f));
    m_tlCameraFrequency.InsertKeyFrame(0, D3DXVECTOR3(1.f, 1.f, 1.f));
    m_tlModelScanl.InsertKeyFrame(0, D3DXVECTOR3(1.f, 1.f, 1.f));
}

KG3DSfxExp::~KG3DSfxExp()
{
    if (m_pBindModel)
        m_pBindModel->SetScaling(&m_vScanlSave);

    KG3DCamera* pCamera = g_cGraphicsTool.GetPrimaryScenePrimaryCamera();

    if (!pCamera)
        pCamera = g_cGraphicsTool.GetCamera();

    if (pCamera)
        pCamera->SetSwingOffset(D3DXVECTOR3(0.f, 0.f, 0.f));
}

BOOL KG3DSfxExp::IsValid()
{
    if (m_tlTimeScal.size() > 1 || m_tlCameraSwing.size() > 1 || m_tlCameraFrequency.size() > 1 || m_tlModelScanl.size() > 1)
        return TRUE;

    D3DXVECTOR3 vData;
    float       fData;

    m_tlTimeScal.GetData(&fData, 0);

    if (fData != 1.f)
        return TRUE;

    m_tlCameraSwing.GetData(&vData, 0);

    if (vData != D3DXVECTOR3(0.f, 0.f, 0.f))
        return TRUE;

    m_tlCameraFrequency.GetData(&vData, 0);

    if (vData != D3DXVECTOR3(1.f, 1.f, 1.f))
        return TRUE;

    m_tlModelScanl.GetData(&vData, 0);

    if (vData != D3DXVECTOR3(1.f, 1.f, 1.f))
        return TRUE;

    return FALSE;
}

BOOL KG3DSfxExp::HaveTimeScaling()
{
    if (m_tlTimeScal.size() > 1)
        return TRUE;

    float fData;
    m_tlTimeScal.GetData(&fData, 0);

    if (fData != 1.f)
        return TRUE;

    return FALSE;
}

void KG3DSfxExp::OnSfxBind(KG3DModel* pModel)
{
    if (!pModel || m_pBindModel == pModel)
        return;

    m_pBindModel = pModel;
    m_pBindModel->GetScaling(&m_vScanlSave);
}

void KG3DSfxExp::OnSfxUnbind()
{
    if (m_pBindModel)
        m_pBindModel->SetScaling(&m_vScanlSave);
    m_pBindModel = NULL;
}

HRESULT KG3DSfxExp::FrameMove(float fCurrentFrame)
{
    if (!g_cGraphicsTool.GetCurScene())
        return E_FAIL;

    float fScaling = 1.f;

    m_tlTimeScal.GetData(&fScaling, fCurrentFrame);
    g_cEngineManager.GetTimeControl().SetScaling(fScaling);

    KG3DCamera* pCamera = g_cGraphicsTool.GetPrimaryScenePrimaryCamera();

    if (!pCamera)
        pCamera = g_cGraphicsTool.GetCamera();

    m_fPassTime += static_cast<float>(g_cGraphicsTool.GetTinyTimeOriginal()) / 1000.f;
    if (HaveTimeScaling())
        m_fPassTime += static_cast<float>(timeGetTime()) / 1000.f;

    if (pCamera)
    {
        D3DXVECTOR3 vSwing;
        D3DXVECTOR3 vFrequ;

        m_tlCameraSwing.GetData(&vSwing, fCurrentFrame);
        m_tlCameraFrequency.GetData(&vFrequ, fCurrentFrame);

        vSwing = vSwing * D3DXVECTOR3(sinf(m_fPassTime * vFrequ.x), sinf(m_fPassTime * vFrequ.y), sinf(m_fPassTime * vFrequ.z));

        pCamera->SetSwingOffset(vSwing);
    }

    D3DXVECTOR3 vScanl;
    m_tlModelScanl.GetData(&vScanl, fCurrentFrame);

    if (m_pBindModel && vScanl != D3DXVECTOR3(1.f, 1.f, 1.f))
    {
        vScanl.x = vScanl.x * m_vScanlSave.x;
        vScanl.y = vScanl.y * m_vScanlSave.y;
        vScanl.z = vScanl.z * m_vScanlSave.z;
     
        m_pBindModel->SetScaling(&vScanl);
    }
   
    return S_OK;
}