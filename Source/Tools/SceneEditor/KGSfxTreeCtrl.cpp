
#include "stdafx.h"
#include "SceneEditor.h"
#include "KGSfxTreeCtrl.h"
#include "IEKG3DSFX.h"
#include "IEEditor.h"

namespace {

    const char* GetNameByPath(const char strPath[])
    {
        static char szName[128];
        _splitpath(strPath, NULL, NULL, szName, NULL);
        return szName;
    }
}

IMPLEMENT_DYNAMIC(KGSfxTreeCtrl, CTreeCtrl)

KGSfxTreeCtrl::KGSfxTreeCtrl() : m_pSfx(NULL)
{

}

KGSfxTreeCtrl::~KGSfxTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(KGSfxTreeCtrl, CTreeCtrl)

END_MESSAGE_MAP()

//&ReadGeneralInformationBlock,   // SFX_FEID_GENERAL_INFORMATION,
//&ReadParticleKindBlock,         // SFX_FEID_PARTICLE_KIND,
//&ReadLauncherBlock,             // SFX_FEID_LAUNCHER,
//&ReadForceFieldBlock,           // SFX_FEID_FORCE_FIELD,
//&ReadTextureBlock,              // SFX_FEID_TEXTURE,
//&ReadBillboardBlock,            // SFX_FEID_BILLBOARD,
//&ReadTrackBlock,                // SFX_FEID_TRACK,
//&ReadModelBlock,                // SFX_FEID_MODEL,
//&ReadMeteorBlock,               // SFX_FEID_METEOR,
//&ReadMagicParpetBlock,          // SFX_FEID_MAGIC_PARPET,
//&ReadChannelBlock,              // SFX_FEID_CHANNEL
//&ReadSoundBlock,                // SFX_FEID_SOUND
//&ReadFootprintBlock,            // SFX_FEID_FOOTPRINT       

void KGSfxTreeCtrl::UpdateUI()
{
    if (!m_pSfx)
        return;

    DeleteAllItems();

    HTREEITEM hRoot = 0;
    HTREEITEM hPrev = 0;
    HTREEITEM hCurr = 0;
    DWORD i;

    if (*(m_pSfx->GetName()))
        hRoot = InsertItem(m_pSfx->GetName());
    else
        hRoot = InsertItem(TEXT("新特效"));

    SetItemData(hRoot, SFX_ELEMENT_SFX);

    // launcher

    if (m_pSfx->GetParticleLauncherNum())
    {
        hPrev = InsertItem(TEXT("发射器"), hRoot);
        SetItemData(hPrev, SFX_ELEMENT_LAU);
    }

    for (i = 0; i < m_pSfx->GetParticleLauncherNum(); ++i)
    {
        IEKG3DSFXParticleLauncher* pLauncher = NULL;
        m_pSfx->GetParticleLauncher(i, &pLauncher);
        hCurr = InsertItem(pLauncher->GetName(), hPrev);
        SetItemData(hPrev, (DWORD)(DWORD_PTR)pLauncher);
    }

    // billboard

    if (m_pSfx->GetBillboardNum())
    {
        hPrev = InsertItem(TEXT("公告牌"), hRoot);
        SetItemData(hPrev, SFX_ELEMENT_BIL);
    }

    for (i = 0; i < m_pSfx->GetBillboardNum(); ++i)
    {
        IEKG3DSFXBillboard* pBillboard = NULL;
        m_pSfx->GetBillboard(i, &pBillboard);
        hCurr = InsertItem(pBillboard->GetName(), hPrev);
        SetItemData(hPrev, (DWORD)(DWORD_PTR)pBillboard);
    }

    // model

    if (m_pSfx->GetNumModels())
    {
        hPrev = InsertItem(TEXT("模型"), hRoot);
        SetItemData(hPrev, SFX_ELEMENT_MDL);
    }

    for (i = 0; i < m_pSfx->GetNumModels(); ++i)
    {
        IEKG3DModel* pModel = NULL;
        LPCTSTR pName = NULL;
        m_pSfx->GetModel(i, &pModel);
        pModel->GetName(&pName);
        hCurr = InsertItem(pName, hPrev);
        SetItemData(hPrev, (DWORD)(DWORD_PTR)pModel);
    }

    // track

    if (m_pSfx->GetTrackNum())
    {
        hPrev = InsertItem(TEXT("轨道"), hRoot);
        SetItemData(hPrev, SFX_ELEMENT_TRA);
    }

    for (i = 0; i < m_pSfx->GetTrackNum(); ++i)
    {
        IEKG3DSFXTrack* pTrack = NULL;
        m_pSfx->GetTrack(i, &pTrack);   
        hCurr = InsertItem(GetNameByPath(pTrack->GetName()), hPrev);
        SetItemData(hPrev, (DWORD)(DWORD_PTR)pTrack);
    }

    // meteor

    if (m_pSfx->GetMeteorNum())
    {
        hPrev = InsertItem(TEXT("刀光"), hRoot);
        SetItemData(hPrev, SFX_ELEMENT_MDL);
    }

    for (i = 0; i < m_pSfx->GetMeteorNum(); ++i)
    {
        IEKG3DSFXMeteor* pMeteor = NULL;
        m_pSfx->GetMeteor(i, &pMeteor);
        hCurr = InsertItem(pMeteor->GetName(), hPrev);
        SetItemData(hPrev, (DWORD)(DWORD_PTR)pMeteor);
    }
}


