////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSFXEditor.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 14:41:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSceneSFXEditor.h"
#include "KG3DSceneWndManager.h"

////////////////////////////////////////////////////////////////////////////////

#include "KG3DSFX.h"
#include "KG3DModelTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModelTable.h"
#include "KG3DEngineManager.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "dsound.h"
#include "KG3DTextureTable.h"
#include "kg3denginemanager.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "PhysiscSystem/KG3DPhysiscManager.h"
#include "IEKG3DAnimationTag.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DTexture.h"
#include "KG3DLocalMoveCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalScalCoord.h"
#include "KG3DSceneCameraMovement.h"

#include "KG3DSceneSelectionTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


static SFX_ENTITY_DATA *_KG_CreateSFXEntityData()
{
    int nResult  = false;
    SFX_ENTITY_DATA *pEntityData = NULL;

    pEntityData = new SFX_ENTITY_DATA;
    KGLOG_PROCESS_ERROR(pEntityData);

    memset(pEntityData, 0, sizeof(*pEntityData));

//Exit1:
    nResult = true;
Exit0:
    return pEntityData;
}

HRESULT KG3DSceneSFXEditor::SetEditState(DWORD dwState)
{
	ULONG uOldState = KG3DSceneEditorBase::GetEditState();
	HRESULT hr = KG3DSceneEditorBase::SetEditState(dwState);
	if (SUCCEEDED(hr) && uOldState != dwState)
	{
		ReleaseGeoElement();
	}
    return hr;
}

void KG3DSceneSFXEditor::SyncBindInfo()
{
	if (!m_pModifySuede || !m_pCurBeBindMdl)
		return;

	m_pModifySuede->ModifyInfo(
		m_nModifyIndex, 
		m_pCurBeBindMdl,
		m_pModifySuede->m_vecBindInfo[m_nModifyIndex].dwBindType,
		m_pModifySuede->m_vecBindInfo[m_nModifyIndex].strTarName
		);
}

void KG3DSceneSFXEditor::CreateGeoElement()
{
    if (!m_lpSFX)
        return;

    ASSERT(!m_pGeoLauncher);    
    switch (m_dwGeoType)
    {
    case SFX_GEO_TYPE_LAUNCHER :
        {
            SFX_LAUNCHER_SHAPE dwShape = SFX_LS_RECTANGLE;
            switch (m_dwGeoSubType)
            {
            case SFX_GEO__LAUNCHER_SUB_TYPE_RECT :
                dwShape = SFX_LS_RECTANGLE;
                break;
            case SFX_GEO__LAUNCHER_SUB_TYPE_CIRC :
                dwShape = SFX_LS_CIRCLE;
                break;
            case SFX_GEO__LAUNCHER_SUB_TYPE_SHER :
                dwShape = SFX_LS_SPHERE;
                break;
            case SFX_GEO__LAUNCHER_SUB_TYPE_CYLR :
                dwShape = SFX_LS_CYLINDER;
                break;
            default :
                ASSERT(false);
                break;
            }
            m_pGeoLauncher = new KG3DSFXParticleLauncher(
                m_lpSFX, 
                &m_lpSFX->m_ParticleContainer, 
                SFX_PT_BILLBOARD_POINT,
                dwShape,
                SFX_PUM_SELF
                );
            ASSERT(m_pGeoLauncher);
        }
        break;
    default  :
        break;
    }
}

void KG3DSceneSFXEditor::AppendGeoElementToSfx()
{
    if (!m_lpSFX)
        return;

    switch (m_dwGeoType)
    {
    case SFX_GEO_TYPE_LAUNCHER :
        if (m_pGeoLauncher)
        {
            m_pGeoLauncher->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(0, m_pGeoLauncher->m_Translation);
            SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
            KG3DSceneEntity Entity(SCENEENTITY_SFX_LAUNCHER, m_pGeoLauncher, pData);
            m_vecLauncherEntityData.push_back(pData);
            m_listRenderEntity.push_back(Entity);

			//this->GetSelectionTool().SetSelectable(Entity, TRUE);//<Selectable Refactor>
			this->IEInsertEntitySelectable(SCENEENTITY_SFX_LAUNCHER, m_pGeoLauncher, TRUE);

            m_lpSFX->m_vecSFXParticleLauncher.push_back(m_pGeoLauncher);
            m_pGeoLauncher = NULL;
        }
        break;
    default :
        break;
    }
    m_dwGeoStep = SFX_GEO_STEP_P;
}

void KG3DSceneSFXEditor::ReleaseGeoElement()
{
    SAFE_DELETE(m_pGeoLauncher);
}

void KG3DSceneSFXEditor::ModifyGeoElement(const D3DXVECTOR3& x, const D3DXVECTOR3& z, const D3DXVECTOR3& y)
{
    if (!m_lpSFX || !m_pGeoLauncher)
        return;

    switch (m_dwGeoType)
    {
    case SFX_GEO_TYPE_LAUNCHER :
        switch (m_dwGeoSubType)
        {
        case SFX_GEO__LAUNCHER_SUB_TYPE_RECT :
        case SFX_GEO__LAUNCHER_SUB_TYPE_SHER :
            {
                D3DXVECTOR3 c;
                if (m_dwGeoSubType == SFX_GEO__LAUNCHER_SUB_TYPE_RECT)
                {
                    m_pGeoLauncher->SetShapeParam1(fabs(x.x - z.x));
                    m_pGeoLauncher->SetShapeParam2(fabs(x.z - z.z));
                    c = (x + z) * 0.5f;
                }
                else
                {
                    m_pGeoLauncher->SetShapeParam1(D3DXVec3Length(&(x -z)));
                    c = x;
                }
                m_pGeoLauncher->SetTranslation(&c);
            }
            break;
        case SFX_GEO__LAUNCHER_SUB_TYPE_CIRC :
            {
                float r1 = D3DXVec3Length(&(z -x));
                float r2 = D3DXVec3Length(&(y - x));
                m_pGeoLauncher->SetTranslation((D3DXVECTOR3*)&x);
                m_pGeoLauncher->SetShapeParam1(max(r1, r2));
                m_pGeoLauncher->SetShapeParam2(min(r1, r2));
            }
            break;
        case SFX_GEO__LAUNCHER_SUB_TYPE_CYLR :
            {
                float r1 = D3DXVec3Length(&(z -x));
                float r2 = y.y - x.y;
                D3DXVECTOR3 c = x;
                c.y += 0.5f * r2;
                if (r2 < 0.f)
                    r2 = -r2;
                m_pGeoLauncher->SetTranslation(&c);
                m_pGeoLauncher->SetShapeParam1(r1);
                m_pGeoLauncher->SetShapeParam2(r2);
            }
            break;
        default :
            break;
        }
        break;
    default :
        break;
    }
}

 
D3DXVECTOR3 KG3DSceneSFXEditor::GetGeoRayPointHor()
{
    D3DXVECTOR3 vr;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXPLANE plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.f);
    m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&v1, &v2, NULL); 
    v2 = v1 + (v2 * 100000.0f);
    D3DXPlaneIntersectLine(&vr, &plane, &v1, &v2);
    return vr;
}

D3DXVECTOR3 KG3DSceneSFXEditor::GetGeoRayPointVel(const D3DXVECTOR3& vHor)
{
    D3DXVECTOR3 vr;
    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    
    D3DXPLANE plane;
    D3DXVECTOR3 vp;
    vp = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraFront();
    vp.y = 0.f;
    D3DXVec3Normalize(&vp, &vp);
    D3DXPlaneFromPointNormal(&plane, &vHor, &vp);
    m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&v1, &v2, NULL); 
    v2 = v1 + (v2 * 100000.0f);
    D3DXPlaneIntersectLine(&vr, &plane, &v1, &v2);
    return vr;
}

int KG3DSceneSFXEditor::GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8])
{
    int nNum = min(maxNum, (int)m_vecRefModels.size());

    for (int i = 0; i < nNum; ++i)
        pModels[i] = m_vecRefModels[i];

    return nNum;
}

int KG3DSceneSFXEditor::ms_BillboardIndex = 0;
int KG3DSceneSFXEditor::ms_LauncherIndex = 0;

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetScene(IEKG3DScene **ppiScene)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiScene);

    *ppiScene = this;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetSceneEditBase(IEKG3DSceneEditorBase **ppiRetSceneEditorBase)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetSceneEditorBase);

    *ppiRetSceneEditorBase = dynamic_cast<IEKG3DSceneEditorBase*>(this);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetSFX(IEKG3DSFX **ppiRetSFX)
{
    HRESULT hResult  = E_FAIL;
    
    KGLOG_PROCESS_ERROR(ppiRetSFX);
    KGLOG_PROCESS_ERROR(m_lpSFX);

    *ppiRetSFX = m_lpSFX;
    
//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetTrackBindInfoLauncherCount(
    DWORD dwIndex, DWORD *pdwRetCount
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecTrackBindInformation.size());
    KGLOG_PROCESS_ERROR(pdwRetCount);

    *pdwRetCount = (DWORD)m_vecTrackBindInformation[dwIndex].vecLauncher.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetTrackBindInfoBillboardCount(
    DWORD dwIndex, DWORD *pdwRetCount
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecTrackBindInformation.size());
    KGLOG_PROCESS_ERROR(pdwRetCount);

    *pdwRetCount = (DWORD)m_vecTrackBindInformation[dwIndex].vecBillboard.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::GetTrackBindInfoModelCount(
    DWORD dwIndex, DWORD *pdwRetCount
    )
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecTrackBindInformation.size());
    KGLOG_PROCESS_ERROR(pdwRetCount);

    *pdwRetCount = (DWORD)m_vecTrackBindInformation[dwIndex].vecModel.size();

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::GetTrackBindInfoMeteorCount(
    DWORD dwIndex, DWORD *pdwRetCount
    )
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecTrackBindInformation.size());
    KGLOG_PROCESS_ERROR(pdwRetCount);

    *pdwRetCount = (DWORD)m_vecTrackBindInformation[dwIndex].vecMeteor.size();

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::UpdateTrackBindInfo()
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    IEKG3DSFXBillboard *pBillboard = NULL;
    DWORD i = 0;

    for (i = 0; i < m_vecTrackBindInformation.size(); ++i)
    {
        m_vecTrackBindInformation[i].vecLauncher.clear();
        m_vecTrackBindInformation[i].vecBillboard.clear();
        m_vecTrackBindInformation[i].vecModel.clear();
        m_vecTrackBindInformation[i].vecMeteor.clear();
    }

    DWORD dwLaucherNum = m_lpSFX->GetParticleLauncherNum();
    for (i = 0; i < dwLaucherNum; ++i)
    {
        m_lpSFX->GetParticleLauncher(i, &pLauncher);
        ASSERT(pLauncher);

        if (pLauncher->IsBindToTrack())
        {
            DWORD dwTrackIndex = pLauncher->GetBindTrackIndex();
            ASSERT(dwTrackIndex < m_lpSFX->GetTrackNum());
            ASSERT(m_lpSFX->GetTrackNum() == m_vecTrackBindInformation.size());

            m_vecTrackBindInformation[dwTrackIndex].vecLauncher.push_back(i);
        }
    }

    DWORD dwBillboardNum = m_lpSFX->GetBillboardNum();
    for (i = 0; i < dwBillboardNum; ++i)
    {
        m_lpSFX->GetBillboard(i, &pBillboard);
        ASSERT(pBillboard);

        if (pBillboard->IsBindToTrack())
        {
            DWORD dwTrackIndex = pBillboard->GetBindTrackIndex();
            ASSERT(dwTrackIndex < m_lpSFX->GetTrackNum());
            ASSERT(m_lpSFX->GetTrackNum() == m_vecTrackBindInformation.size());

            m_vecTrackBindInformation[dwTrackIndex].vecBillboard.push_back(i);
        }
    }

    DWORD dwModelNum = m_lpSFX->GetNumModels();
    for (i = 0; i < dwModelNum; ++i)
    {
        if (m_lpSFX->m_vecModelsInfo[i].pSFXModel && m_lpSFX->m_vecModelsInfo[i].nTrackIndex != -1)
        {
             if (m_lpSFX->m_vecModelsInfo[i].nTrackIndex >= (INT)m_lpSFX->GetTrackNum())
             {
                 m_lpSFX->m_vecModelsInfo[i].nTrackIndex = -1;
                 continue;
             }
             ASSERT(m_lpSFX->GetTrackNum() == m_vecTrackBindInformation.size());
             m_vecTrackBindInformation[m_lpSFX->m_vecModelsInfo[i].nTrackIndex].vecModel.push_back(i);
        }        
    }

    DWORD dwMeteorNum = m_lpSFX->GetMeteorNum();
    for (i = 0; i < dwMeteorNum; ++i)
    {
        if (m_lpSFX->m_vecMeteor[i] && m_lpSFX->m_vecMeteor[i]->GetBindTrackIdx() != -1)
        {
            if (m_lpSFX->m_vecMeteor[i]->GetBindTrackIdx() >= (INT)m_lpSFX->GetTrackNum())
            {
                m_lpSFX->m_vecMeteor[i]->SetBindTrackIdx(-1);
                continue;
            }
            ASSERT(m_lpSFX->GetTrackNum() == m_vecTrackBindInformation.size());
            m_vecTrackBindInformation[m_lpSFX->m_vecMeteor[i]->GetBindTrackIdx()].vecMeteor.push_back(i);
        }        
    }

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::LauncherBindToTrack(
    DWORD dwLauncherIndex, DWORD dwTrackIndex
)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSFXParticleLauncher *pLauncher = NULL;
    DWORD dwBindTrackIndex = 0;
    DWORD i = 0;

    KGLOG_PROCESS_ERROR(dwTrackIndex < m_vecTrackBindInformation.size());

    m_lpSFX->GetParticleLauncher(dwLauncherIndex, &pLauncher);
    ASSERT(pLauncher);

    dwBindTrackIndex = pLauncher->GetBindTrackIndex();
    if (dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
    {
        for (i = 0; i < m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher[i] == (int)dwLauncherIndex)
            {
                m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.erase(
                    m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.begin() + i
                );
                break;
            }
        }

        if (i > m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.size())
            ASSERT(0);
    }

    pLauncher->BindToTrack(dwTrackIndex);
    m_vecTrackBindInformation[dwTrackIndex].vecLauncher.push_back(dwLauncherIndex);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::LauncherUnbindFromTrack(DWORD dwLauncherIndex)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSFXParticleLauncher *pLauncher = NULL;

    m_lpSFX->GetParticleLauncher(dwLauncherIndex, &pLauncher);
    ASSERT(pLauncher);

    DWORD dwBindTrackIndex = pLauncher->GetBindTrackIndex();
    if (dwBindTrackIndex != -1)
    {
        for (int i = 0; i < (int)m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher[i] == (int)dwLauncherIndex)
            {
                m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.erase(
                    m_vecTrackBindInformation[dwBindTrackIndex].vecLauncher.begin() + i
                );

                pLauncher->UnBindToTrack();

                break;
            }
        }

        //@#$%^& who add this!!
        //ASSERT(0);
    }

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::BillboardBindToTrack(
    DWORD dwBillboardIndex, DWORD dwTrackIndex
)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSFXBillboard *pBillboard = NULL;
    DWORD dwBindTrackIndex = 0;
    DWORD i = 0;

    KGLOG_PROCESS_ERROR(dwTrackIndex < m_vecTrackBindInformation.size());

    m_lpSFX->GetBillboard(dwBillboardIndex, &pBillboard);
    ASSERT(pBillboard);

    dwBindTrackIndex = pBillboard->GetBindTrackIndex();
    if (dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
    {
        for (i = 0; i < m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard[i] == (int)dwBillboardIndex)
            {
                m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.erase(
                    m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.begin() + i
                );

                break;
            }
        }

        if (i > m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.size())
            ASSERT(0);
    }

    pBillboard->BindToTrack(dwTrackIndex);
    m_vecTrackBindInformation[dwTrackIndex].vecBillboard.push_back(dwBillboardIndex);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::BillboardUnbindFromTrack(DWORD dwBillboardIndex)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSFXBillboard *pBillboard = NULL;
    DWORD dwBindTrackIndex = 0;
    DWORD i = 0;

    m_lpSFX->GetBillboard(dwBillboardIndex, &pBillboard);
    ASSERT(pBillboard);

    dwBindTrackIndex = pBillboard->GetBindTrackIndex();
    if (dwBindTrackIndex != SFX_NOT_BIND_TO_TRACK)
    {
        for (i = 0; i < m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard[i] == (int)dwBillboardIndex)
            {
                m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.erase(
                    m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.begin() + i
                );

                pBillboard->UnBindToTrack();

                break;
            }
        }

        if (i > m_vecTrackBindInformation[dwBindTrackIndex].vecBillboard.size())
            ASSERT(0);
    }

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}


HRESULT KG3DSceneSFXEditor::ModelBindToTrack(
    DWORD dwModelIndex, DWORD dwTrackIndex
)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwBindIndex = 0;

    KG_PROCESS_ERROR(dwTrackIndex < m_vecTrackBindInformation.size());
    KG_PROCESS_ERROR(m_lpSFX);
    KG_PROCESS_ERROR(dwModelIndex < m_lpSFX->GetNumModels());

    dwBindIndex = m_lpSFX->m_vecModelsInfo[dwModelIndex].nTrackIndex;

    if (dwBindIndex != -1)
    {
        size_t i = 0;
        for (i = 0; i < m_vecTrackBindInformation[dwBindIndex].vecModel.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindIndex].vecModel[i] == (int)dwModelIndex)
            {
                m_vecTrackBindInformation[dwBindIndex].vecModel.erase(
                    m_vecTrackBindInformation[dwBindIndex].vecModel.begin() + i
                    );

                break;
            }
        }
        if (i > m_vecTrackBindInformation[dwBindIndex].vecModel.size())
            ASSERT(0);
    }

    m_lpSFX->m_vecModelsInfo[dwModelIndex].nTrackIndex = dwTrackIndex;
    m_vecTrackBindInformation[dwTrackIndex].vecModel.push_back(dwModelIndex);

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::ModeldUnbindFromTrack(DWORD dwModelIndex)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwBindIndex = 0;

    KG_PROCESS_ERROR(m_lpSFX);
    KG_PROCESS_ERROR(dwModelIndex < m_lpSFX->GetNumModels());

    dwBindIndex = m_lpSFX->m_vecModelsInfo[dwModelIndex].nTrackIndex;

    if (dwBindIndex != -1)
    {
        size_t i = 0;
        for (i = 0; i < m_vecTrackBindInformation[dwBindIndex].vecModel.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindIndex].vecModel[i] == (int)dwModelIndex)
            {
                m_vecTrackBindInformation[dwBindIndex].vecModel.erase(
                    m_vecTrackBindInformation[dwBindIndex].vecModel.begin() + i
                    );

                m_lpSFX->m_vecModelsInfo[dwModelIndex].nTrackIndex = -1;

                break;
            }
        }
        if (i > m_vecTrackBindInformation[dwBindIndex].vecModel.size())
            ASSERT(0);
    }

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::MeteorBindToTrack(DWORD dwMeteorIndex, DWORD dwTrackIndex)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwBindIndex = 0;

    KG_PROCESS_ERROR(dwTrackIndex < m_vecTrackBindInformation.size());
    KG_PROCESS_ERROR(m_lpSFX);
    KG_PROCESS_ERROR(dwMeteorIndex < m_lpSFX->GetMeteorNum());

    dwBindIndex = m_lpSFX->m_vecMeteor[dwMeteorIndex]->GetBindTrackIdx();

    if (dwBindIndex != -1)
    {
        size_t i = 0;
        for (i = 0; i < m_vecTrackBindInformation[dwBindIndex].vecMeteor.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindIndex].vecMeteor[i] == (int)dwMeteorIndex)
            {
                m_vecTrackBindInformation[dwBindIndex].vecMeteor.erase(
                    m_vecTrackBindInformation[dwBindIndex].vecMeteor.begin() + i
                    );

                break;
            }
        }
        if (i > m_vecTrackBindInformation[dwBindIndex].vecMeteor.size())
            ASSERT(0);
    }

    m_lpSFX->m_vecMeteor[dwMeteorIndex]->SetBindTrackIdx(dwTrackIndex);
    m_vecTrackBindInformation[dwTrackIndex].vecMeteor.push_back(dwMeteorIndex);

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;


}

HRESULT KG3DSceneSFXEditor::MeteorUnbindFromTrack(DWORD dwMeteorIndex)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwBindIndex = 0;

    KG_PROCESS_ERROR(m_lpSFX);
    KG_PROCESS_ERROR(dwMeteorIndex < m_lpSFX->GetMeteorNum());

    dwBindIndex = m_lpSFX->m_vecMeteor[dwMeteorIndex]->GetBindTrackIdx();

    if (dwBindIndex != -1)
    {
        size_t i = 0;
        for (i = 0; i < m_vecTrackBindInformation[dwBindIndex].vecMeteor.size(); ++i)
        {
            if (m_vecTrackBindInformation[dwBindIndex].vecMeteor[i] == (int)dwMeteorIndex)
            {
                m_vecTrackBindInformation[dwBindIndex].vecMeteor.erase(
                    m_vecTrackBindInformation[dwBindIndex].vecMeteor.begin() + i
                    );

                m_lpSFX->m_vecMeteor[dwMeteorIndex]->SetBindTrackIdx(-1);

                break;
            }
        }
        if (i > m_vecTrackBindInformation[dwBindIndex].vecMeteor.size())
            ASSERT(0);
    }

    //Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetBindModel(IEKG3DModel **ppiRetBindModel)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetBindModel);

    *ppiRetBindModel = m_pBindModel;
//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetRefrenceModel(int index, IEKG3DModel **ppiRetRefrenceModel)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetRefrenceModel);
    KGLOG_PROCESS_ERROR(index < (int)m_vecRefModels.size());

    *ppiRetRefrenceModel = m_vecRefModels[index];
//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::RefModelsSeekAnimation(DWORD dwSeekType, FLOAT fOffset)
{
    HRESULT hResult  = E_FAIL;
    IKG3DAnimationController *piAnimationControlller = NULL;

    for (DWORD i = 0; i < m_vecRefModels.size(); i++)
    {
        piAnimationControlller = m_vecRefModels[i]->GetAnimationController(ANICTL_PRIMARY);
        if (piAnimationControlller)
            piAnimationControlller->SeekAnimation(dwSeekType, (int)fOffset);
    }

    UpdateSfxExp();

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::RefModelsPlaySFX(
    SFX_ANIMATION_PLAY SFXModelAnimationMode, 
    BOOL bAnotherModelPauseAnimation
)
{
    HRESULT hResult  = E_FAIL;
    IKG3DAnimationController *piAnimationControlller = NULL;
    size_t uSzie = 0;
    unsigned int uModelType = MESHTYPE_DEFAULT;
    KG3DSFX *pSFX = NULL;

    uSzie = m_vecRefModels.size();

    for (size_t i = 0; i < uSzie; ++i)
    {
        uModelType = m_vecRefModels[i]->GetType();

        if (uModelType == MESHTYPE_SFX)
        {
            pSFX = dynamic_cast<KG3DSFX *>(m_vecRefModels[i]);
            pSFX->PlayAnimation(SFXModelAnimationMode, 1.f, (int)pSFX->GetCurrentFrame() + 1);
        }
        else 
        {
            piAnimationControlller = m_vecRefModels[i]->GetAnimationController(ANICTL_PRIMARY);
            if (piAnimationControlller)
                piAnimationControlller->PauseAnimation(bAnotherModelPauseAnimation);
        }
    }

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetCurBindModel(IEKG3DModel **ppiRetCurBindModel)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetCurBindModel);

    *ppiRetCurBindModel = m_pCurBindModel;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetCurBeBindModel(IEKG3DModel **ppiRetCurBeBindModel)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetCurBeBindModel);

    *ppiRetCurBeBindModel = m_pCurBeBindMdl;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::SetBindTargerInfo(INT nType, LPCTSTR pcszName)
{
    HRESULT hResult  = E_FAIL;
    static const size_t BIND_TARGER_INFO_NAME_CHAR_COUNT =
        sizeof(m_bindTargerInfo.szName) / sizeof(m_bindTargerInfo.szName[0]);

    KGLOG_PROCESS_ERROR(pcszName);

    m_bindTargerInfo.nType = nType;
    strncpy(
        m_bindTargerInfo.szName,
        pcszName,
        BIND_TARGER_INFO_NAME_CHAR_COUNT - 1
    );
    m_bindTargerInfo.szName[BIND_TARGER_INFO_NAME_CHAR_COUNT - 1] = '\0';

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetBindTargerInfoName(LPCTSTR *ppcszRetName)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppcszRetName);

    *ppcszRetName = m_bindTargerInfo.szName;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetModelNameBySocketName(
    LPCTSTR pcszSocketName, LPCTSTR *ppcszRetModelName
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pcszSocketName);
    KGLOG_PROCESS_ERROR(ppcszRetModelName);

    for (DWORD i = 0; i < m_vecITEM.size(); i++)
    {
        if (m_vecITEM[i].strSocketName == pcszSocketName)
        {
            m_vecITEM[i].pItemModel->GetName(ppcszRetModelName);
            KG_PROCESS_SUCCESS(true);
        }
    }
    KGLOG_PROCESS_ERROR(false && "Can''t found the socket name");

Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetBillboardEntityDataCount(DWORD *pdwBillboardCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwBillboardCount);

    *pdwBillboardCount = (DWORD)m_vecBillboardEntityData.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetBillboardEntityData(
    DWORD dwIndex, SFX_ENTITY_DATA **ppRetBillboardEntityData
    )
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecBillboardEntityData.size());
    KGLOG_PROCESS_ERROR(ppRetBillboardEntityData);

    *ppRetBillboardEntityData = m_vecBillboardEntityData[dwIndex];

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::SetBillboardEntityDataNameToDefault(
    SFX_ENTITY_DATA *pBillboardEntityData
)
{
    HRESULT hResult  = E_FAIL;
    int nRetCode = 0;
    static const size_t NAME_BUFFER_CHAR_COUNT = 
        sizeof(pBillboardEntityData->szEntityName) / 
        sizeof(pBillboardEntityData->szEntityName[0]);

    KGLOG_PROCESS_ERROR(pBillboardEntityData);

    nRetCode = snprintf(
        pBillboardEntityData->szEntityName, 
        NAME_BUFFER_CHAR_COUNT,
        "公告板%d", 
        ++ms_BillboardIndex
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    pBillboardEntityData->szEntityName[NAME_BUFFER_CHAR_COUNT - 1] = '\0';

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetLauncherEntityDataCount(DWORD *pdwLauncherCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwLauncherCount);

    *pdwLauncherCount = (DWORD)m_vecLauncherEntityData.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::GetLauncherEntityData(
    DWORD dwIndex, SFX_ENTITY_DATA **ppRetLauncherEntityData
)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecLauncherEntityData.size());
    KGLOG_PROCESS_ERROR(ppRetLauncherEntityData);

    *ppRetLauncherEntityData = m_vecLauncherEntityData[dwIndex];

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::AddLauncherEntityData()
{
    HRESULT hResult  = E_FAIL;

    SFX_ENTITY_DATA *pEntity = NULL;
    
    pEntity = _KG_CreateSFXEntityData();
    KGLOG_PROCESS_ERROR(pEntity);

    m_vecLauncherEntityData.push_back(pEntity);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::SetLauncherEntityDataNameToDefault(
    SFX_ENTITY_DATA *pLauncherEntityData
)
{
    HRESULT hResult  = E_FAIL;
    int nRetCode = 0;
    static const size_t NAME_BUFFER_CHAR_COUNT = 
        sizeof(pLauncherEntityData->szEntityName) / 
        sizeof(pLauncherEntityData->szEntityName[0]);

    KGLOG_PROCESS_ERROR(pLauncherEntityData);

    nRetCode = snprintf(
        pLauncherEntityData->szEntityName, 
        NAME_BUFFER_CHAR_COUNT,
        "发射器%d", 
        ++ms_BillboardIndex
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    pLauncherEntityData->szEntityName[NAME_BUFFER_CHAR_COUNT - 1] = '\0';


//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////

HRESULT KG3DSceneSFXEditor::EnableSfxCloseFloor(int nEnable)
{
	m_lpSFX->m_nCloseFloorFlag = nEnable;
	return S_OK;
}

HRESULT KG3DSceneSFXEditor::EnableRenterLauncher(int nBeRender)
{
    m_bRenderLauncherFlag = nBeRender;
    return S_OK;
}


HRESULT KG3DSceneSFXEditor::GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet)
{
	if (NULL != pfRet)
	{
		*pfRet = 0;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSFXEditor::UpdateBindOrRefrenceModel(LPSTR szPath)
{
    HRESULT hResult  = E_FAIL;
    KG_PROCESS_ERROR(szPath);
    if (m_dwSceneMode == SFX_SCENE_MODE_BIND)
        LoadBindModel(szPath);
    else if (m_dwSceneMode == SFX_SCENE_MODE_EDIT)
        LoadRefrenceModel(szPath);
    else
        ASSERT(false);

    if (m_bIsModify)
        LoadRefrenceModel(szPath);

Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::UpdateBindOrRefrenceAni(LPSTR szPath)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    IKG3DAnimationController* pAniController = NULL;
    FLOAT  fSFXCurFrame  = 0;
    DWORD  dwSFXPlayMode = 0;
    DWORD  dwMDLPlayMode = 0;
    DWORD  dwSFXFrameDly = 0;

    KG3DSceneEntity Entity;
    KG3DModel*   pSelModel = NULL;

    //KG_PROCESS_ERROR(m_listSelectedEntity.size());

    //Entity = *(m_listSelectedEntity.begin());
    //KG_PROCESS_ERROR(Entity.m_Type == SCENEENTITY_MODEL);

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
    if (/*m_listSelectedEntity.size()*/selTool.GetSelectionCount())
    {
		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		proxy.Begin();

        //Entity = *(m_listSelectedEntity.begin());
		Entity = proxy.GetCur();
        KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);
    }
    else
    {
		//选中的物体中有没有任何的Model
		struct CustomVisitor : public KG3DSceneEntityVisitor 
		{
			size_t m_uModelCount;
			CustomVisitor():m_uModelCount(0){}
			virtual HRESULT Accept(KG3DSceneEntity& entity)
			{
				if(entity.GetType() == SCENEENTITY_MODEL)
				{
					++m_uModelCount;
					return E_FAIL;	//提前跳出
				}
				return S_OK;
			}
		};
		CustomVisitor customVisitor;
		this->GetSelectionTool().TraverseSelectable(customVisitor);
		KG_PROCESS_ERROR(customVisitor.m_uModelCount > 0);

        /*size_t size = m_listSelectableEntity.size();
        KG_PROCESS_ERROR(size);

        KG3DSceneEntityList::iterator i;

        for (i = m_listSelectableEntity.begin(); i != m_listSelectableEntity.end(); i++)
        {
            Entity = *i;

            if (Entity.GetType() == SCENEENTITY_MODEL)
                break;
        }

        KG_PROCESS_ERROR(i != m_listSelectableEntity.end());*/
    }


    pSelModel = (KG3DModel*)Entity.m_pPointer;
    KG_PROCESS_ERROR(pSelModel);

    KG_PROCESS_ERROR(m_lpSFX);

    fSFXCurFrame  = m_lpSFX->GetCurrentFrame();
    dwMDLPlayMode = m_lpSFX->GetModelPlayMode();
    dwSFXPlayMode = m_lpSFX->GetPlayAnimation();
    dwSFXFrameDly = m_lpSFX->GetFrameInterval();

    IEKG3DAnimationTagContainer *pContainer = NULL;
	pSelModel->GetAnimationTagContainer(&pContainer);
	if (pContainer && pContainer->IsLoaded())
	{
		pContainer->Start(1.0f, 0, ENU_ANIMATIONPLAY_ONCE, ANICTL_PRIMARY);
	}
	else
	{
		hRetCode = pSelModel->PlayAnimation(
        dwMDLPlayMode, szPath, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY
		);
	    KG_COM_PROCESS_ERROR(hRetCode);
	}

    pAniController = pSelModel->GetAnimationController(ANICTL_PRIMARY);
    KG_PROCESS_ERROR(pAniController);

    pAniController->SeekAnimation(ANIMATION_SEEK_SET, (int)(fSFXCurFrame * dwSFXFrameDly));

    if (dwSFXPlayMode == SFX_AP_PAUSE)
        pAniController->PauseAnimation(TRUE);
    else
        pAniController->PauseAnimation(FALSE);

    hResult = S_OK;
Exit0:
    return hResult;
}

// 
//  ========特效编辑器可以处于两种状态========
//  编辑状态 : SFX_SCENE_MODE_EDIT
//  绑定状态 : SFX_SCENE_MODE_BIND
//  两个状态通过 UpdateSceneMode 切换,各自保存
//  自己的数据, 各自不影响 n_n
//  ========ZhaoXinYu=============================
//
HRESULT KG3DSceneSFXEditor::UpdateSceneMode(DWORD dwMode)
{
    HRESULT hResult  = E_FAIL;

   /* KG_PROCESS_ERROR(dwMode == SFX_SCENE_MODE_EDIT || dwMode == SFX_SCENE_MODE_BIND);
    KG_PROCESS_ERROR(dwMode != m_dwSceneMode);

    switch (m_dwSceneMode)
    {
    case SFX_SCENE_MODE_EDIT :
        m_listPostRenderEdit   = m_listPostRenderEntity;
        m_listSceneEntityEdit  = m_listRenderEntity;
        m_listRenderEntity     = m_listSceneEntityBind;
        m_listPostRenderEntity = m_listPostRenderBind;
        m_listSelectableEntity = m_listSceneEntityBind;
        break;
    case SFX_SCENE_MODE_BIND :
        m_listPostRenderBind   = m_listPostRenderEntity;
        m_listSceneEntityBind  = m_listRenderEntity;
        m_listRenderEntity     = m_listSceneEntityEdit;
        m_listPostRenderEntity = m_listPostRenderEdit;
        m_listSelectableEntity = m_listSceneEntityEdit;
        break;
    default :
        KG_PROCESS_ERROR(false);
        break;
    }
    m_listSelectedEntity.clear();*/
    m_pCurBindModel  = NULL;
    m_pCurBeBindMdl  = NULL;
    m_nLinkingFlag   = BIND_STATE_FREE;
    m_dwSceneMode = dwMode;
    hResult = S_OK;
//Exit0:
    return hResult;
}

KG3DSceneSFXEditor::KG3DSceneSFXEditor(void)
{
	m_lpSFX = NULL;
    m_pModel = NULL;
    //m_dwEditState = SCENESTATE_SELECT;
	KG3DSceneEditorBase::SetEditState(SCENESTATE_SELECT);
    m_bFocus = FALSE;

    m_pModelEntityData = NULL;

	m_dwSubEditState = SCENESTATE_SUB_NONE;

    m_pBindModel     = NULL;
   // m_pRefrenceModel = NULL;
    //m_pBindFlagMesh  = NULL;
//    m_pSFXSuede      = NULL;
    m_pCurBindModel  = NULL;
    m_pCurBeBindMdl  = NULL;

    m_bRenderLauncherFlag = TRUE;
    m_lpD3DFont = NULL;

	m_dwPrevTime  = timeGetTime();
	m_dwCurrTime  = timeGetTime();
	*m_szInfo     = '\0';
	m_fCounter    = 0;

    m_nLinkingFlag = BIND_STATE_FREE;

    m_nShowVideoInfo = FALSE;
    //m_lpCalkBackRender = NULL;
    m_dwSceneMode = SFX_SCENE_MODE_EDIT;

    m_listSceneEntityBind.clear();
    m_listSceneEntityBind.clear();

    m_pGroundGridVertexs = NULL;
    m_pModifySuede = NULL;
//    m_pMfyRdfModel = NULL;

    m_dwTimer     = 0x0;
    m_fFPS        = 500.f;
    m_pHandModel  = NULL;
    m_nCurBindSfxIndex = -1;
    m_vecRefModels.clear();

    m_bIsModify      = FALSE;

    m_dwBkColor      = 0x007f7f7f;

    m_dwGeoType    = 0xffffffff;
    m_dwGeoSubType = 0xffffffff;
    m_dwGeoStep    = SFX_GEO_STEP_P;
    m_pGeoLauncher = NULL;


    m_dwD3DFillModel = D3DFILL_SOLID;
	m_pBGTexture = NULL;
	m_bShowBGTexture = FALSE;
    m_pEffect = NULL;
    m_pGroundTexture = NULL;
    m_pScanlModel = NULL;

}

KG3DSceneSFXEditor::~KG3DSceneSFXEditor(void)
{
    SAFE_RELEASE(m_pHandModel);
    SAFE_RELEASE(m_lpSFX);
    SAFE_RELEASE(m_pModel);
    SAFE_RELEASE(m_pBindModel);
   // SAFE_RELEASE(m_pRefrenceModel);
   /* SAFE_RELEASE(m_pCurBindModel);
    SAFE_RELEASE(m_pCurBeBindMdl);*/
    SAFE_RELEASE(m_lpD3DFont);
    SAFE_RELEASE(m_pGroundGridVertexs);
    SAFE_RELEASE(m_pModifySuede);
    SAFE_RELEASE(m_pGroundTexture);

    SAFE_DELETE(m_pGeoLauncher);
    SAFE_RELEASE(m_pScanlModel);
    SAFE_RELEASE(m_pEffect);
//    SAFE_DELETE(m_pSFXSuede);

    for (int i = 0; i < (int)m_vecBillboardEntityData.size(); ++i)
    {
        SAFE_DELETE(m_vecBillboardEntityData[i]);
    }

    m_vecBillboardEntityData.clear();

    for (int i = 0; i < (int)m_vecLauncherEntityData.size(); ++i)
    {
        SAFE_DELETE(m_vecLauncherEntityData[i]);
    }

    m_vecLauncherEntityData.clear();

    for (int i = 0; i < (int)m_vecTrackEntityData.size(); ++i)
    {
        SAFE_DELETE(m_vecTrackEntityData[i]);
    }

    m_vecTrackEntityData.clear();

    for (size_t i = 0; i < m_vecITEM.size(); i++)
        SAFE_RELEASE(m_vecITEM[i].pItemModel);
    m_vecITEM.clear();        

    for (size_t i = 0; i < m_vecRefModels.size(); i++)
        SAFE_RELEASE(m_vecRefModels[i]);
    m_vecRefModels.clear();

    SAFE_DELETE(m_pModelEntityData);
}

HRESULT KG3DSceneSFXEditor::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn)
{
    *pOut = *pIn;
    return S_OK;
}

HRESULT KG3DSceneSFXEditor::SaveToFile(const char lpszPathName[])
{
	if (FAILED(m_lpSFX->SaveToFile(lpszPathName, 0)))
		return E_FAIL;


	FILE *pFile = NULL;

	// Billboard
	string strNameFile = string(lpszPathName) + ".billboardname";
	fopen_s(&pFile, strNameFile.c_str(), "w");
	if (!pFile)
		return ERROR_CONTINUE;

	fprintf(pFile, "%d\n", m_vecBillboardEntityData.size());
	for (int i = 0; i < (int)m_vecBillboardEntityData.size(); ++i)
	{
		fprintf(pFile, "%s\n", m_vecBillboardEntityData[i]->szEntityName);
	}
	fclose(pFile);

	// Launcher
	strNameFile = string(lpszPathName) + ".launchername";
	fopen_s(&pFile, strNameFile.c_str(), "w");
	if (!pFile)
		return ERROR_CONTINUE;

	fprintf(pFile, "%d\n", m_vecLauncherEntityData.size());
	for (int i = 0; i < (int)m_vecLauncherEntityData.size(); ++i)
	{
		fprintf(pFile, "%s\n", m_vecLauncherEntityData[i]->szEntityName);
	}
	fclose(pFile);

	// Track maybe, fix me~~~!

	return S_OK;
}

HRESULT KG3DSceneSFXEditor::LoadBindFile(const char cszBindFile[])
{
  /*  if (!m_pBindModel)
        return E_FAIL;

    KG3DSFXSuede *psuede = (KG3DSFXSuede*)m_pBindModel->GetSFXSuede();
    if (!psuede)
        return E_FAIL;

    HRESULT hr = psuede->LoadFromBindFile(m_pBindModel, cszBindFile);
    if (FAILED(hr))
        return E_FAIL;

    DWORD cnt = (DWORD)psuede->GetSFXCount();
    for (DWORD i = 0; i < cnt; ++i)
    {
        KG3DSFX* psfx = (KG3DSFX*)psuede->GetSFX(i);
        ASSERT(psfx->GetType() == MESHTYPE_SFX);
        KG3DSceneEntity e(SCENEENTITY_MODEL, psfx);
        if (psfx->m_nIsSockWave)
        {
            m_listPostRenderEntity.remove(e);
            m_listPostRenderEntity.push_back(e);
        }
        else
        {
            m_listRenderEntity.remove(e);
            m_listRenderEntity.push_back(e);
        }
        m_listSelectableEntity.remove(e);
        m_listSelectableEntity.push_back(e);
        psfx->PlayAnimation(SFX_AP_LOOP);
    }*/
    return S_OK;
}

HRESULT KG3DSceneSFXEditor::SaveBindFile(const char cszFileName[])
{
    HRESULT hResult = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(m_pBindModel);
    KGLOG_PROCESS_ERROR(cszFileName);

    IEKG3DSFXSuede *piSFX = NULL;
    KG3DSFXSuede *pSuede = NULL;
    m_pBindModel->GetSFXSuede(&piSFX);
    pSuede = dynamic_cast<KG3DSFXSuede *>(piSFX);
    
    KGLOG_PROCESS_ERROR(pSuede);

    hRetCode = pSuede->SaveToFile(cszFileName);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::OpenSFX(LPCTSTR pFileName)
{
	SAFE_RELEASE(m_lpSFX);

    /*
	g_cModelTable.Get1NewResource((KG3DModel**)&m_lpSFX, MESHTYPE_SFX, 0);

	if (FAILED(m_lpSFX->LoadFromFile(pFileName, 0, 0)))
        return E_FAIL;
    */

    if (FAILED(g_cModelTable.LoadResourceFromFile(pFileName, 0, MLO_SFX_EDITING, (IKG3DResourceBase**)&m_lpSFX)))
        return E_FAIL;


    m_lpSFX->PlayAnimation(SFX_AP_PAUSE);
    m_lpSFX->EnableBillboardSort(FALSE);
    m_lpSFX->SetCurrentFrame(0);
    
    if (!m_lpSFX->m_pExpEffect)
        m_lpSFX->m_pExpEffect = new KG3DSfxExp();

	for (int i = 0; i < (int)m_lpSFX->GetBillboardNum(); ++i)
	{
		KG3DSFXBillboard *pBillboard = NULL;
		m_lpSFX->GetBillboard(i, &pBillboard);
		ASSERT(pBillboard);

        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        m_vecBillboardEntityData.push_back(pData);
		//KG3DSceneEntity Entity(SCENEENTITY_SFX_BILLBOARD, pBillboard, pData);//<Selectable Refactor>
		//selTool.SetSelectable(Entity, TRUE);
		this->IEInsertEntitySelectable(SCENEENTITY_SFX_BILLBOARD, pBillboard, TRUE);
	}

    for (int i = 0; i < (int)m_lpSFX->GetParticleLauncherNum(); ++i)
    {
        KG3DSFXParticleLauncher *pLauncher = NULL;
        m_lpSFX->GetParticleLauncher(i, &pLauncher);
		ASSERT(pLauncher);

        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        m_vecLauncherEntityData.push_back(pData);

        KG3DSceneEntity Entity(SCENEENTITY_SFX_LAUNCHER, pLauncher, pData);
        m_listRenderEntity.push_back(Entity);
        
		//selTool.SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_SFX_LAUNCHER, pLauncher, TRUE);
    }

    for (int i = 0; i < (int)m_lpSFX->GetTrackNum(); ++i)
    {
        KG3DSFXTrack *pTrack = NULL;
        m_lpSFX->GetTrack(i, &pTrack);
        ASSERT(pTrack);

        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        m_vecTrackEntityData.push_back(pData);
        KG3DSceneEntity Entity(SCENEENTITY_SFX_TRACK, pTrack, pData);
        m_listRenderEntity.push_back(Entity);
        
		//selTool.SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_SFX_TRACK, pTrack, TRUE);


        SFX_TRACK_BIND_INFORMATION info;
        m_vecTrackBindInformation.push_back(info);
    }

    for (size_t s = 0; s < m_lpSFX->GetNumModels(); s++)
    {
		//KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_lpSFX->GetModel(s));//<Selectable Refactor>        
		//selTool.SetSelectable(Entity, TRUE);
		this->IEInsertEntitySelectable(SCENEENTITY_MODEL, m_lpSFX->GetModel(s), TRUE);

        m_lpSFX->GetModel(s)->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
        IKG3DAnimationController* pController = m_lpSFX->GetModel(s)->GetAnimationController(ANICTL_PRIMARY);
        pController->PauseAnimation(TRUE);
    }


    for (int i = 0; i < (int)m_lpSFX->GetMeteorNum(); ++i)
    {
        KG3DSFXMeteor *pMeteor = NULL;
        m_lpSFX->GetMeteor(i, (IEKG3DSFXMeteor**)&pMeteor);
        ASSERT(pMeteor);

		KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, pMeteor);
        m_listRenderEntity.push_back(Entity);
		
        
		//selTool.SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_SFX_METEOR, pMeteor, TRUE);
    }

    for (int i = 0; i < (int)m_lpSFX->GetBoxProjectionNum(); ++i)
    {
        KG3DSfxBoxProjection* bp = m_lpSFX->m_vecBoxProj[i];
        KG3DSceneEntity Entity(SCENEENTITY_SFX_BOX_PROJ, bp);
        m_listRenderEntity.push_back(Entity);
        this->IEInsertEntitySelectable(SCENEENTITY_SFX_BOX_PROJ, bp, TRUE);
    }

    KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_lpSFX);
    if (m_lpSFX->m_nIsSockWave)
    {
        m_listPostRenderEntity.push_back(Entity);
        if (m_lpSFX->m_dwShockMode == SHOCK_WAVE_ADD)
            m_listRenderEntity.push_back(Entity); 
    }
    else
        m_listRenderEntity.push_back(Entity); 

	FILE* pFile = NULL;
	// Billboard
	string strNameFile = string(pFileName) + ".billboardname";
	fopen_s(&pFile, strNameFile.c_str(), "r");
	if (!pFile)
		return ERROR_CONTINUE;

	int nNum = 0;
	fscanf(pFile, "%d\n", &nNum);
	if (nNum != (int)m_lpSFX->GetBillboardNum())
		return ERROR_INVALID_DATA;
 
	for (int i = 0; i < (int)m_lpSFX->GetBillboardNum(); ++i)
	{
        static const size_t NAME_BUFFER_CHAR_COUNT = 
            sizeof(m_vecBillboardEntityData[0]->szEntityName) / 
            sizeof(m_vecBillboardEntityData[0]->szEntityName[0]);

		fscanf_s(
            pFile, 
            "%s\n", 
            m_vecBillboardEntityData[i]->szEntityName, 
            NAME_BUFFER_CHAR_COUNT
        );
		m_vecBillboardEntityData[i]->szEntityName[NAME_BUFFER_CHAR_COUNT - 1] = '\0';
	}
	fclose(pFile);

	// Launcher
	strNameFile = string(pFileName) + ".launchername";
	fopen_s(&pFile, strNameFile.c_str(), "r");
	if (!pFile)
		return ERROR_CONTINUE;

	fscanf_s(pFile, "%d\n", &nNum, sizeof(int));
	if (nNum != (int)m_lpSFX->GetParticleLauncherNum())
		return ERROR_INVALID_DATA;

	for (DWORD i = 0; i < m_lpSFX->GetParticleLauncherNum(); ++i)
	{
        static const size_t NAME_BUFFER_CHAR_COUNT = 
            sizeof(m_vecLauncherEntityData[0]->szEntityName) / 
            sizeof(m_vecLauncherEntityData[0]->szEntityName[0]);

		fscanf_s(pFile, "%s\n", m_vecLauncherEntityData[i]->szEntityName, NAME_BUFFER_CHAR_COUNT);
		m_vecLauncherEntityData[i]->szEntityName[NAME_BUFFER_CHAR_COUNT - 1] = '\0';
	}
	fclose(pFile);

	// Track Maybe, fix me ~~!!!


    //
//    m_pSFXSuede = new KG3DSFXSuede;

	return S_OK;
}

HRESULT KG3DSceneSFXEditor::NewSFX()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    SAFE_RELEASE(m_lpSFX);

    hRetCode = g_cModelTable.Get1NewResource((KG3DModel**)&m_lpSFX, MESHTYPE_SFX, 0);
    KG_COM_PROCESS_ERROR(hRetCode);

    m_lpSFX->EnableBillboardSort(FALSE);
    m_lpSFX->SetCurrentFrame(0);
    m_lpSFX->PlayAnimation(SFX_AP_PAUSE);
    m_lpSFX->m_bMeshPostLoadProcessed = TRUE;
	m_lpSFX->SetTexture(0, "\\data\\public\\errortexture.tga");
    
    if (!m_lpSFX->m_pExpEffect)
        m_lpSFX->m_pExpEffect = new KG3DSfxExp();

    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_lpSFX);
        m_listRenderEntity.push_back(Entity);
    }

    hResult = S_OK;
Exit0 :
    return hResult;
}

HRESULT KG3DSceneSFXEditor::RegistCalkBackRender(RenderCallBack* pRender)
{
    if (!pRender)
        return E_FAIL;

    //m_lpCalkBackRender = pRender;
    m_lpCalkBackRender.push_back(pRender);
    return S_OK;
}

HRESULT KG3DSceneSFXEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	switch(pWnd->GetType())
	{
	case OUTPUTWND_SFXEDITOR_MAIN:
		RenderForMainWindow(wndCur);
		break;
    case OUTPUTWND_RESOURCE_MINIVIEW :
        for (DWORD i = 0; i < m_lpCalkBackRender.size(); i++)
        {
            if (m_lpCalkBackRender[i] && m_lpCalkBackRender[i]->wndID == pWnd->m_nID)
                m_lpCalkBackRender[i]->Render(pWnd);
        }
        break;
	default:
		break;
	}
	return S_OK;
}

HRESULT KG3DSceneSFXEditor::SetFocus(BOOL bFocus)
{
    m_bFocus = bFocus;
    return S_OK;
}

HRESULT KG3DSceneSFXEditor::GetFocus(BOOL *pbRetFocus)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pbRetFocus);

    *pbRetFocus = m_bFocus;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::ShowBindBone()
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/ 0 != this->GetSelectionTool().GetSelectionCount());   
    /*for (
        KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
        i != m_listSelectedEntity.end();
        i++
    )
    {*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity& entity = proxy.GetCur();

        if (entity.GetType() == SCENEENTITY_MODEL)
        {
            KG3DModel* pModel = (KG3DModel*)entity.m_pPointer;
            if (pModel->IsShowBones())
            {
                pModel->ShowBones(FALSE);
            }
            else
            {
                pModel->ShowBones(TRUE);
            }
            pModel->UpdateBoneMesh(this);
        }
    }
    
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::UpdateBoneSel()
{
    HRESULT hResult  = E_FAIL;

   /* KG3DSceneEntity entity;
    KG3DModel* pModel = NULL;

    KG_PROCESS_ERROR(m_listSelectedEntity.size() == 1);
    entity = *m_listSelectedEntity.begin();
    KG_PROCESS_ERROR(entity.m_Type == SCENEENTITY_MODEL);*/
    KG_PROCESS_ERROR(m_pCurBindModel);

    //pModel = (KG3DModel*)entity.m_pPointer;
    m_pCurBindModel->UpdateBoneMeshSel();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::_AdhereInfo::Update(D3DXVECTOR3* pPosition)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DXMATRIX matPosition;
    int  nBoneSockIndex = 0;
    BindExtraInfo Info;
    
    KG_PROCESS_ERROR(pPosition);
    KG_PROCESS_ERROR(pModel);

    //KG_PROCESS_ERROR(!IsBadCodePtr((FARPROC)pModel));

    switch(dwType)
    {
    case 0 :    // origin
        matPosition = pModel->m_matWorld;
        break;  
    case 1 :    // bone
        nBoneSockIndex = pModel->FindBone(strEffectPointName.c_str(), &Info);
        KG_PROCESS_ERROR(nBoneSockIndex != -1);

        if (pModel->GetType() == MESHTYPE_MODELST)
            hRetCode = Info.pModel->GetBoneMatrix(Info.nBindIndex, &matPosition, NULL);
        else
            hRetCode = pModel->GetBoneMatrix(nBoneSockIndex, &matPosition, NULL);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        break;
    case 2 :    // socket
        nBoneSockIndex = pModel->FindSocket(strEffectPointName.c_str(), &Info);
        KG_PROCESS_ERROR(nBoneSockIndex != -1);
        if (pModel->GetType() == MESHTYPE_MODELST)
            hRetCode = Info.pModel->GetSocketMatrix(Info.nBindIndex, matPosition);
        else
            hRetCode = pModel->GetSocketMatrix(nBoneSockIndex, matPosition);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        break;
    default :
        matPosition = pModel->m_matWorld;
        break;
    }

    pPosition->x = matPosition._41;
    pPosition->y = matPosition._42;
    pPosition->z = matPosition._43;

    hResult = S_OK;
Exit0:
   /* if (hResult != S_OK && ppModel && *ppModel)
    {
        matPosition = (*ppModel)->m_matWorld;
        pPosition->x = matPosition._41;
        pPosition->y = matPosition._42;
        pPosition->z = matPosition._43;
        hResult = S_OK;
    }*/
    return hResult;
}

HRESULT KG3DSceneSFXEditor::AdhereToModel(
    KG3DModel* pSourceModel, string strSurEFName, DWORD dwSurType,
    KG3DModel* pTargetModel, string strTarEFName, DWORD dwTarType 
)
{
    HRESULT hResult  = E_FAIL;

    if (g_bClient)
    {
        KG_PROCESS_ERROR(pSourceModel);
        KG_PROCESS_ERROR(pTargetModel);
    }

    m_adereSource.strEffectPointName = strSurEFName;
    m_adereSource.dwType = dwSurType;
    m_adereSource.pModel = pSourceModel;

    m_adereTarget.strEffectPointName = strTarEFName;
    m_adereTarget.dwType = dwTarType;
    m_adereTarget.pModel = pTargetModel;

    hResult = S_OK;
Exit0:
    return hResult;
}

//HRESULT KG3DSceneSFXEditor::GetLinkEntity(
//    _RayLine         originRayLine,
//    _RayLine         targetRayLine, 
//    KG3DSceneEntity& originEntity,
//    KG3DSceneEntity& targetEntity 
//)
//{
//    HRESULT hResult  = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//
//    D3DXVECTOR3 vCorss[128];
//    DWORD dwCrossCount = 0;
//
//    std::KG3DSceneEntityList::iterator iter = m_listRenderEntity.begin();
//    while (iter != m_listRenderEntity.end())
//    {
//        if ((*iter).GetRayIntersect(dwCrossCount, vCorss, originRayLine.vOrg, originRayLine.vDir))
//        {
//            int a = 0;
//            a++;
//        }
//        iter++;
//    }
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}

HRESULT KG3DSceneSFXEditor::LoadHandModel(LPSTR pPath)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG_PROCESS_ERROR(pPath);
    
    SAFE_RELEASE(m_pHandModel);
    hRetCode = g_cModelTable.LoadResourceFromFile(
        pPath, 0, 0, (IKG3DResourceBase **)&m_pHandModel
    );
    KG_COM_PROCESS_ERROR(hRetCode);
	
	//打开刀光特效取样////////////////////////////////////////////
	if (m_pHandModel->GetType() == MESHTYPE_SFX)
	{
		KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(m_pHandModel);
		pSFX->SetMeteorSampling(TRUE);
        pSFX->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, "", 1.f, 0, NULL, NULL, ANICTL_PRIMARY);
	}
	/////////////////////////////////////////////////////////////
    hResult = S_OK;
Exit0:
    if (hResult != S_OK)
        SAFE_RELEASE(m_pHandModel);
    return hResult;
}

HRESULT KG3DSceneSFXEditor::SetCurBindModel()
{
    HRESULT hResult  = E_FAIL;

    KG3DSceneEntity Entity;
    KG3DModel*   pSelModel = NULL;

    KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/0 != this->GetSelectionTool().GetSelectionCount());

    //Entity = *(m_listSelectedEntity.begin());

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	proxy.Begin();
	Entity = proxy.GetCur();

    KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);

    pSelModel = (KG3DModel*)Entity.m_pPointer;
    KG_PROCESS_ERROR(pSelModel);

    m_pCurBindModel = pSelModel;

    m_bindTargerInfo.ReSet();

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::DrawBindFlag()
{
    HRESULT hResult  = E_FAIL;

   // KG3DSceneEntity Entity;
   // KG3DModel*   pSelModel = NULL;
   // D3DXMATRIX   matWordSave;
   // D3DXMATRIX   matPostition;
   // D3DXVECTOR3  vA;
   // D3DXVECTOR3  vB;
   // D3DMATERIAL9 malFlag;

   // //KG_PROCESS_ERROR(m_pBindFlagMesh);

   ///* KG_PROCESS_ERROR(m_listSelectedEntity.size());

   // Entity = *(m_listSelectedEntity.begin());
   // KG_PROCESS_ERROR(Entity.m_Type == SCENEENTITY_MODEL);

   // pSelModel = (KG3DModel*)Entity.m_pPointer;*/
   // KG_PROCESS_ERROR(m_pCurBindModel);

   // matPostition = m_pCurBindModel->m_matWorld;
   // m_pCurBindModel->GetBBox(vA, vB);
   // matPostition._42 = vB.y + 18;

   // ZeroMemory(&malFlag, sizeof(malFlag));

   // malFlag.Diffuse.a = 1.0f;
   // malFlag.Diffuse.r = 1.0f;
   // malFlag.Diffuse.g = 0.0f;
   // malFlag.Diffuse.b = 0.0f;

   // malFlag.Ambient  = malFlag.Diffuse;
   // malFlag.Specular = malFlag.Diffuse;

   // g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWordSave);
   // g_pd3dDevice->SetTransform(D3DTS_WORLD, &matPostition);
   // g_pd3dDevice->SetMaterial(&malFlag);
   // m_pBindFlagMesh->DrawSubset(0);
   // g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWordSave);

    hResult = S_OK;
//Exit0:
    return hResult;
}


HRESULT KG3DSceneSFXEditor::AddHandMdlToRefMdl()
{
    HRESULT hResult  = E_FAIL;
    KG_PROCESS_ERROR(m_pHandModel);

    m_vecRefModels.push_back(m_pHandModel);
    
    m_pHandModel->GetAnimationController(ANICTL_PRIMARY)->PauseAnimation(TRUE);
    
    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pHandModel);

        m_listRenderEntity.remove(Entity);
        
		//this->GetSelectionTool().SetSelectable(Entity, FALSE);//<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_MODEL, m_pHandModel, FALSE);

        m_listPostRenderEntity.remove(Entity);
        
        if (m_pHandModel->m_nIsSockWave)
        {
            m_listPostRenderEntity.push_back(Entity);
            if (m_pHandModel->m_dwShockMode == SHOCK_WAVE_ADD)
                m_listRenderEntity.push_back(Entity);
        }
        else
            m_listRenderEntity.push_back(Entity);

		//this->GetSelectionTool().SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_MODEL, m_pHandModel, TRUE);
    }


    m_pHandModel = NULL;

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSceneSFXEditor::UpdateSelRefMdl()
{
    HRESULT hResult  = E_FAIL;

    KG3DSceneEntity entity;
    KG3DModel* pModel = NULL;

	KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/0 != this->GetSelectionTool().GetSelectionCount());
    //entity = *m_listSelectedEntity.begin();
	entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();

    pModel = (KG3DModel*)entity.m_pPointer;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::FrameMove()
{
	g_cGraphicsTool.SetCurScene(this);
    HRESULT hRetCode = E_FAIL;

    D3DXVECTOR3 vEffectPos1;
    D3DXVECTOR3 vEffectPos2;

    KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX);

    KG_PROCESS_ERROR(m_lpSFX);

    if (std::find(m_listRenderEntity.begin(), m_listRenderEntity.end(), entity) == m_listRenderEntity.end())
        m_lpSFX->FrameMove();

    if (m_pGeoLauncher)
        m_pGeoLauncher->FrameMove();

    hRetCode = m_adereSource.Update(&vEffectPos1);
    if (hRetCode != S_OK)
        vEffectPos1 = D3DXVECTOR3(-500.0f, 100.0f, 0.0f);
    hRetCode = m_adereTarget.Update(&vEffectPos2);
    if (hRetCode != S_OK)
        vEffectPos2 = D3DXVECTOR3(500.0f, 100.0f, 0.0f);
    m_lpSFX->UpdateChannelEffectPos(vEffectPos1, vEffectPos2);

    g_ProjTextureRender.RemoveAllProjSource();
    g_ProjTextureRender.CheckInputProjectSource(m_lpSFX);


    if (::GetFocus() == m_SceneWndManager->GetCurOutputWnd()->m_hWnd)
        m_bEnableMoveCam = TRUE;
    else
        m_bEnableMoveCam = FALSE;




Exit0:
    return KG3DSceneEditorBase::FrameMove();
}

#if 0
int index = 0;
void CallbackRenderTest(DWORD dwIndex, void* pUserData)
{
    IDirect3DSurface9* pSurfaceSave = NULL; 
    IDirect3DSurface9* pSurface = NULL;
    KG3DTexture* pTexture = NULL;
    DWORD dwResourceId = 0;
    D3DVIEWPORT9 ViewPortSave;
    g_pd3dDevice->GetViewport(&ViewPortSave);

    g_pd3dDevice->GetRenderTarget(0, &pSurfaceSave);
    HRESULT hr = g_cTextureTable.GetResource(&pTexture, dwIndex);
    
    if (SUCCEEDED(hr) &&　pTexture)
    {
        pTexture->Release();
		pTexture->GetTexture()->GetSurfaceLevel(0, &pSurface);
        if (pSurfaceSave)
            g_pd3dDevice->SetRenderTarget(0, pSurface);
        else
            g_pd3dDevice->SetRenderTarget(0, pSurfaceSave);
    }
    g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, 0xFFFF0000, 1, 0);

    g_pd3dDevice->SetRenderTarget(0, pSurfaceSave);
    g_pd3dDevice->SetViewport(&ViewPortSave);

    KG_COM_RELEASE(pSurface);
    KG_COM_RELEASE(pSurfaceSave);
}

#endif

HRESULT KG3DSceneSFXEditor::ShockWaveRender()
{
    KG3DRenderState RS;
   // if (SUCCEEDED(pWnd->ShockWaveRenderTargetSet()))
    {
        RS.SetRenderState(D3DRS_ZENABLE, TRUE);
        KG3DSceneEntityList::iterator i = m_listPostRenderEntity.begin();
        while (i != m_listPostRenderEntity.end())
        {
            i->FrameMove();
            i->RenderOption(RENDER_SHOCKWAVE);
            ++i;
        }

    }

    return S_OK;
}

HRESULT KG3DSceneSFXEditor::RenderForMainWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
    KG3DRenderState RenderState;

    if(m_lpPhysicsScene)
    {
        IKG3DCamera* pCamera = GetCurrentCamera();
        if (pCamera)
        {
            D3DXVECTOR3 vLookAtPos;
            D3DXVECTOR3 vCameraPos;
            pCamera->GetLookAtPosition(&vLookAtPos);
            pCamera->GetPosition(&vCameraPos);
            m_lpPhysicsScene->FrameMove((DWORD)g_cEngineManager.GetTimeControl().GetTinyTime(), vCameraPos, vLookAtPos);
        }
    }

    RenderWindowBegin(pWnd,TRUE);

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, m_dwBkColor, 1.f, 0);

    RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
    RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
    RenderState.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	RenderState.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

    RenderState.SetRenderState(D3DRS_FILLMODE, m_dwD3DFillModel);
    RenderState.SetRenderState(D3DRS_ZENABLE, TRUE);

	if (m_pBGTexture && m_bShowBGTexture)
		g_cGraphicsTool.DrawBackGroudTexture(m_pBGTexture->GetTexture());

	g_cGraphicsTool.ApplyLight(*this);
	g_SetShaderSharedParams(*this, wndCur, NULL, NULL);
    //g_ProjTextureRender.Render(&pWnd->GetCamera());
    DrawGroundGrid(&pWnd->GetCamera());
    //g_ProjTextureRender.RemoveAllProjSource();

    if (m_pHandModel)
    {
        RECT rectWnd;
        POINT ptCursor;

        GetWindowRect(pWnd->m_hWnd, &rectWnd);
        GetCursorPos(&ptCursor);

        if (PtInRect(&rectWnd, ptCursor))
        {
            D3DXVECTOR3 vPlace;
            D3DXVECTOR3 v1;
            D3DXVECTOR3 v2;
            static D3DXPLANE plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.f);

            pWnd->GetPickRay(&v1, &v2, NULL); 
            v2 = v1 + (v2 * 100000.0f);
            D3DXPlaneIntersectLine(&vPlace, &plane, &v1, &v2);
            
            m_pHandModel->SetTranslation(&vPlace);
        
        }

        m_pHandModel->FrameMove();	
        m_pHandModel->Render(0, NULL);

        if (m_pHandModel->GetType() == MESHTYPE_SFX)
        {
            KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(m_pHandModel);
            pSFX->RenderFootprint();
        }

        
    }
 
    KG3DSceneEntityList::iterator iter = m_listRenderEntity.begin();
    while (iter != m_listRenderEntity.end())
    {
        if (iter->m_pPointer == m_lpSFX)
        {
            iter++;
            continue;
        }

        if (iter->GetType() == SCENEENTITY_MODEL)
        {
            KG3DModel* pModel = reinterpret_cast<KG3DModel*>(iter->m_pPointer);
            KG3DModel* pParentModel = NULL;

            if (pModel)
            {				
                if (pModel->IsShowBones())
                    pModel->UpdateBoneMesh(this);

                pModel->GetParentModel((IKG3DModel**)&pParentModel);

                if (!pParentModel)
                    iter->Render();

				if (pModel->GetType() == MESHTYPE_SFX)
				{
					KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(pModel);
					pSFX->RenderFootprint();
				}
            }
        }
        else if (iter->GetType() == SCENEENTITY_SFX_LAUNCHER || iter->GetType() == SCENEENTITY_SFX_TRACK)
        {
            if (m_bRenderLauncherFlag)
                iter->Render();
        }
        else
        {
            iter->Render();
        }
        iter++;
    }

    if (m_lpSFX->CheckNeedRender())
	{
		m_lpSFX->Render(0, NULL);
		m_lpSFX->RenderFootprint();
	}

    if (m_pGeoLauncher)
        m_pGeoLauncher->EditorRender();

    if (m_bRenderLauncherFlag)
    {
        D3DXMATRIX Proj,View;
        Proj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
        View = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();

		if(this->GetEditState() != SCENESTATE_SFX_GEO)
		{
			RenderSelectedEntityMark(this->GetEditState(), m_SceneWndManager->GetCurOutputWnd()->m_Viewport, Proj,View, TRUE );
		}
    }

    if (m_bFocus)
    {
        g_cGraphicsTool.DrawFocusRect(
            m_SceneWndManager->GetCurOutputWnd()->m_Viewport, 
            4,
            0xFFFFFF00
            );
    }

    g_cGraphicsTool.DrawLeftHandCoordinate();
    if (m_nShowVideoInfo)
        g_cGraphicsTool.DrawVideoInfo(50, 58, 0xFFFFFFFF);

    if (m_nLinkingFlag == BIND_STATE_LINKING && m_dwSceneMode == SFX_SCENE_MODE_BIND && !m_bIsModify)
    {
		if (m_SceneWndManager->GetCurOutputWnd())
		{
			ASSERT(m_pCurBeBindMdl);
			D3DXVECTOR3 vOrigin = m_pCurBeBindMdl->m_BBox.GetCenter();
			D3DXVECTOR3 vTarget = D3DXVECTOR3(1000, 1000, 0);
			D3DXVECTOR3 vDirect = D3DXVECTOR3(1000, 1000, 0);
			m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&vTarget, &vDirect, NULL);
			vTarget += vDirect * 100;
			g_cGraphicsTool.DrawLine(&vOrigin, &vTarget, 0xFFFFFFFF, 0xFFFFFFFF);
		}
    }

	m_dwCurrTime = timeGetTime();

	while (m_dwCurrTime - m_dwPrevTime < (1000 / 45))
	{
		Sleep(1);
		m_dwCurrTime = timeGetTime();
	}

    m_dwTimer   += m_dwCurrTime - m_dwPrevTime;
    m_fCounter  += 1.f;

    if (m_dwTimer > 1000)
    {
        m_fFPS     = m_fCounter * 1000.f / static_cast<FLOAT>(m_dwTimer);
        m_fCounter = 0.f;
        m_dwTimer  = 0;
    }

    static RECT rectFPS = {5, 5, 0, 0}; 
    sprintf(m_szInfo, TEXT("FPS : %.2f"), m_fFPS);

    m_lpD3DFont->DrawText( NULL, m_szInfo, -1, &rectFPS, DT_NOCLIP, 0xFFFFFFFF );

    if (m_lpSFX && m_lpD3DFont)
    {
        float fHeight = (float)m_SceneWndManager->GetCurOutputWnd()->m_Viewport.Height;
        static DWORD dwMaxParticleNum = 0;

        if (m_lpSFX->m_eSFXAnimationPlay == SFX_AP_PAUSE)
            dwMaxParticleNum = 0;
        dwMaxParticleNum = max(dwMaxParticleNum, m_lpSFX->m_ParticleContainer.m_dwActiveNumber / 2);

        RECT rectInfo;
        SetRect(&rectInfo, 5, (int)(fHeight - 20), 0, 0);

        TCHAR szInfo[128];
        sprintf(
            szInfo, TEXT("Particel Num : %4u    Max Particel Num : %4u    BillBoard Num : %4u "), 
            m_lpSFX->m_ParticleContainer.m_dwActiveNumber / 2,
            dwMaxParticleNum,
            m_lpSFX->GetBillboardNum()
        );
        m_lpD3DFont->DrawText( NULL, szInfo, -1, &rectInfo, DT_NOCLIP, 
            0xFFFFFFFF );
    }

    if (m_lpD3DFont)
    {
        for (DWORD i = 0; i < m_vecRefModels.size(); i++)
        {
            float fWidth = (float)m_SceneWndManager->GetCurOutputWnd()->m_Viewport.Width;
            RECT  rectInfo;
            DWORD dwType;
            SetRect(&rectInfo, (int)(fWidth - 200), 5, 0, 0);
            TCHAR szInfo[128];
            TCHAR szBoneName[128];
            *szInfo = '\0';
            *szBoneName = '\0';
            
            if (m_vecRefModels[i] && m_vecRefModels[i]->IsShowBones())
            {
                m_vecRefModels[i]->GetBoneMesh()->GetSelBoneName(
                    szBoneName, sizeof(szBoneName), &dwType
                );
                sprintf(
                    szInfo, TEXT("Sel : %s"), 
                    szBoneName
                    );
                m_lpD3DFont->DrawText( 
                    NULL, 
                    szInfo,
                    -1, 
                    &rectInfo, 
                    DT_NOCLIP, 
                    0xFFFFFFFF 
                );
                break;
            }
        }
    }

    if (pWnd->IsEnablePostRender())
    {
		GraphicsStruct::RenderState RSFog(D3DRS_FOGENABLE,FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

        pWnd->GetPostRenderManager()->RenderAoe();
        pWnd->RenderPostEffect(this);
    }

	m_dwPrevTime = m_dwCurrTime;

    IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();

	if (pSoundShell && g_cEngineManager.GetSoundFocusScene() == this)
	{
		D3DXVECTOR3 vec3Front, vec3Up(.0f, 1.0f, .0f);
		D3DXVECTOR3 vPos = pWnd->GetCamera().GetPosition() / 100.f;
		GetCurrentCamera()->GetFront(&vec3Front);
		D3DXVECTOR3 vVel = D3DXVECTOR3(0.f, 0.f, 0.f);
		pSoundShell->SetListenerPosition(&vPos, &vVel);
		pSoundShell->SetOrientation(&vec3Front, &vec3Up);
	}

    RenderWindowEnd(pWnd);

	return S_OK;
}


void KG3DSceneSFXEditor::DrawGroundGrid(KG3DCamera* pCamera)
{
    const int nGridNum = 40;
    UINT uVertexNum = (nGridNum + 1) * 4;

    struct _Vertex  {
        D3DXVECTOR3 v;
        DWORD       c;
    };

    if (!m_pGroundGridVertexs)
    {
        HRESULT hr = g_pd3dDevice->CreateVertexBuffer(
            uVertexNum * sizeof(_Vertex),
            0,
            D3DFVF_XYZ | D3DFVF_DIFFUSE,
            D3DPOOL_MANAGED,
            &m_pGroundGridVertexs,
            NULL
        );
        if (SUCCEEDED(hr))
        {
            _Vertex* pVertexs = NULL;

            m_pGroundGridVertexs->Lock(0, 0, (void**)&pVertexs, 0);

            int strick = (nGridNum + 1);
            for (int i = 0; i < (nGridNum + 1); i ++)
            {
                pVertexs[i * 2].v     = D3DXVECTOR3(-2000.f, 0.f, 2000.f - i * 100.f);
                pVertexs[i * 2 + 1].v = D3DXVECTOR3( 2000.f, 0.f, 2000.f - i * 100.f);
                pVertexs[i * 2].c     = 0xFF606060;
                pVertexs[i * 2 + 1].c = 0xFF606060;
            }
            for (int i = (nGridNum + 1); i < (nGridNum + 1) * 2; i++)
            {
                pVertexs[i * 2].v     = D3DXVECTOR3(2000.f - (i - strick) * 100.f, 0.f,  2000.f);
                pVertexs[i * 2 + 1].v = D3DXVECTOR3(2000.f - (i - strick) * 100.f, 0.f, -2000.f);
                pVertexs[i * 2].c     = 0xFF606060;
                pVertexs[i * 2 + 1].c = 0xFF606060;
            }

            pVertexs[nGridNum].c = 0xFF000000;
            pVertexs[nGridNum + 1].c = 0xFF000000;
            pVertexs[(nGridNum + 1) * 3 - 1].c = 0xFF000000;
            pVertexs[(nGridNum + 1) * 3].c = 0xFF000000;
            m_pGroundGridVertexs->Unlock();
        }
        else
        {
            SAFE_RELEASE(m_pGroundGridVertexs);
        }

    }

    if (m_pGroundGridVertexs)
    {
        KG3DRenderState RenderState;
        RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

        if (m_pGroundTexture)
        {

            D3DXMATRIX matWVP = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();
            UINT uPass = 0;

            D3DXVECTOR3 lt = D3DXVECTOR3(-2000.f, -0.5f,  2000.f);
            D3DXVECTOR3 rt = D3DXVECTOR3( 2000.f, -0.5f,  2000.f);
            D3DXVECTOR3 lb = D3DXVECTOR3(-2000.f, -0.5f, -2000.f);

            if (m_pEffect)
            {
                m_pEffect->SetTechnique("tec");
                m_pEffect->SetMatrix("WorldViewProj", &matWVP);
                g_ProjTextureRender.SetEffectParam(m_pEffect);
                m_pEffect->SetTexture("ColTexture", m_pGroundTexture ? m_pGroundTexture->GetTexture() : NULL);
                m_pEffect->Begin(&uPass, 0);
                m_pEffect->BeginPass(0);
                g_cGraphicsTool.DrawRect(&lt, &rt, &lb, 0xffffffff , NULL, 10.f, 10.f);
                m_pEffect->EndPass();
                m_pEffect->End();
            }
            else
            {
                g_cGraphicsTool.DrawRect(&lt, &rt, &lb, 0xffffffff , m_pGroundTexture ? m_pGroundTexture->GetTexture() : NULL, 10.f, 10.f);
            }
        }
        else
        {
            g_pd3dDevice->SetTexture(0, NULL);
            g_pd3dDevice->SetStreamSource(0, m_pGroundGridVertexs,0, sizeof(_Vertex));
            g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
            g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, (nGridNum + 1) * 2);
        }
    }

}


HRESULT KG3DSceneSFXEditor::ChangeTrack(DWORD dwIndex, const char cszNewFileName[])
{
    KG3DSFXTrack *pTrack = NULL;
    HRESULT hr = m_lpSFX->GetTrack(dwIndex, &pTrack);
    if (FAILED(hr))
        return E_FAIL;

    hr = m_lpSFX->ChangeTrack(dwIndex, cszNewFileName);
    return hr;
}

HRESULT KG3DSceneSFXEditor::AddTrack(
    const char cszFileName[],
    float fX, float fY, float fZ
)
{
    KG3DSFXTrack *pTrack = NULL;
    HRESULT hr = m_lpSFX->AddTrack(cszFileName, &pTrack);
    if (SUCCEEDED(hr))
    {
        pTrack->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        KG3DSceneEntity Entity(SCENEENTITY_SFX_TRACK, pTrack, pData);
        m_vecTrackEntityData.push_back(pData);
        m_listRenderEntity.push_back(Entity); 
        
		//this->GetSelectionTool().SetSelectable(Entity, TRUE);//<Selectable Refactor>
		this->InsertEntitySelectable(Entity, TRUE);

        SFX_TRACK_BIND_INFORMATION info;
        m_vecTrackBindInformation.push_back(info);
    }

    return hr;
}

HRESULT KG3DSceneSFXEditor::AddModel(const char cszFileName[], float fX, float fY, float fZ)
{
    KG3DModel *pModel = NULL;
    HRESULT hr = E_FAIL;
    //hr = RemoveModel();
    //KG_COM_PROCESS_ERROR(hr);

    hr = m_lpSFX->AddModel(cszFileName, &pModel);
    KG_COM_PROCESS_ERROR(hr);

    {
        pModel->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
        m_pModelEntityData = _KG_CreateSFXEntityData();
        KG_PROCESS_ERROR(m_pModelEntityData);

        //KG3DSceneEntity e(SCENEENTITY_MODEL, m_lpSFX->GetModel());
        //m_listSelectedEntity.remove(e);
        //m_listSelectableEntity.remove(e);

        KG3DSceneEntity Entity(SCENEENTITY_MODEL, pModel, m_pModelEntityData);
		/*this->GetSelectionTool().SetSelectable(Entity, TRUE);*///<Selectable Refactor>		
		this->InsertEntitySelectable(Entity, TRUE);

        m_listRenderEntity.push_back(Entity);
    }

    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DSceneSFXEditor::RemoveModel(size_t index)
{
    KG3DModel *pModel = m_lpSFX->GetModel(index);
    HRESULT hr = m_lpSFX->RemoveModel(index);

    if (SUCCEEDED(hr))
    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, pModel);
        
		//m_listSelectableEntity.remove(Entity);
		this->GetSelectionTool().SetSelectable(Entity, FALSE);
        
		//m_listSelectedEntity.remove(Entity);
		this->GetSelectionTool().SetSelected(Entity, FALSE);
        m_listRenderEntity.remove(Entity);
        m_listPostRenderEntity.remove(Entity);

        SAFE_DELETE(m_pModelEntityData);
    }
    return hr;
}

HRESULT KG3DSceneSFXEditor::ChangeModel(const char cszNewFileName[], size_t index)
{
    KG3DModel *pModel = NULL;
    KG3DModel *pOldModel = m_lpSFX->GetModel(index);
    if (pOldModel == NULL)
        return E_FAIL;

    HRESULT hr = m_lpSFX->SetModel(cszNewFileName, &pModel, index);
    if (SUCCEEDED(hr))
    {
        KG3DSceneEntity EntityOld(SCENEENTITY_MODEL, pOldModel);
        
		//m_listSelectableEntity.remove(EntityOld);
		this->GetSelectionTool().SetSelectable(EntityOld, FALSE);

        //m_listSelectedEntity.remove(EntityOld);
		this->GetSelectionTool().SetSelected(EntityOld, FALSE);

        m_listRenderEntity.remove(EntityOld);

        m_pModelEntityData = _KG_CreateSFXEntityData();
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, pModel, m_pModelEntityData);
        
		//m_listSelectableEntity.push_back(Entity);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);

        m_listRenderEntity.push_back(Entity);
    }

    return hr;
}

HRESULT KG3DSceneSFXEditor::AddBillboard(float fX, float fY, float fZ)
{
    KG3DSFXBillboard *pBillboard = NULL;
    HRESULT hr = m_lpSFX->AddBillboard(&pBillboard);
    if (SUCCEEDED(hr))
    {
        pBillboard->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
        pBillboard->m_vPositionLine.InsertKeyFrame(0, D3DXVECTOR3(fX, fY, fZ));
        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        KG3DSceneEntity Entity(SCENEENTITY_SFX_BILLBOARD, pBillboard, pData);
        m_vecBillboardEntityData.push_back(pData);
        
		//m_listSelectableEntity.push_back(Entity);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);
    }

    return hr;
}

HRESULT KG3DSceneSFXEditor::AddBoxProjection()
{
    ASSERT(m_lpSFX);
    IKG3DSfxBoxProjection* pBoxProjection = NULL;
    m_lpSFX->AddBoxProjection(&pBoxProjection);

    KG3DSceneEntity Entity(SCENEENTITY_SFX_BOX_PROJ, pBoxProjection, 0);
    this->GetSelectionTool().SetSelectable(Entity, TRUE);

    return S_OK;
}

HRESULT KG3DSceneSFXEditor::RemoveBoxProjection(DWORD dwIndex)
{
    ASSERT(m_lpSFX);

    KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, m_lpSFX->GetBoxProjection(dwIndex));

    this->GetSelectionTool().SetSelectable(Entity, FALSE);
    m_listRenderEntity.remove(Entity);
    this->GetSelectionTool().SetSelected(Entity, FALSE);

    return m_lpSFX->RemoveBoxProjection((int)dwIndex);
}

HRESULT KG3DSceneSFXEditor::AddMeteor()
{
    HRESULT hr = E_FAIL;
    KG3DSFXMeteor *pMeteor = NULL;
    hr = m_lpSFX->AddMeteor(&pMeteor);
    if (SUCCEEDED(hr))
    {
        KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, pMeteor);
        m_listRenderEntity.push_back(Entity);
        
		//m_listSelectableEntity.push_back(Entity);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);
    }

    return hr;
}

HRESULT KG3DSceneSFXEditor::RemoveMeteor(DWORD dwIndex)
{
    KG3DSFXMeteor *pMeteor = NULL;
    IEKG3DSFXMeteor *piMeteor = NULL;

    if (FAILED(m_lpSFX->GetMeteor(dwIndex, &piMeteor)))
        return E_FAIL;
    pMeteor = dynamic_cast<KG3DSFXMeteor *>(piMeteor);

    KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, pMeteor);
    
	//m_listSelectableEntity.remove(Entity);
	this->GetSelectionTool().SetSelectable(Entity, FALSE);

    m_listRenderEntity.remove(Entity);
    //m_listSelectedEntity.remove(Entity);
	this->GetSelectionTool().SetSelected(Entity, FALSE);

    return m_lpSFX->RemoveMeteor(dwIndex);
}

HRESULT KG3DSceneSFXEditor::AddLauncher(float fX, float fY, float fZ)
{
    KG3DSFXParticleLauncher *pLauncher = NULL;
    HRESULT hr = m_lpSFX->AddParticleLauncher(
        SFX_PT_BILLBOARD_POINT,
        SFX_LS_RECTANGLE,
        SFX_PUM_SELF,
        &pLauncher);
    if (SUCCEEDED(hr))
    {
        pLauncher->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(0, D3DXVECTOR3(fX, fY, fZ));
        pLauncher->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
        pLauncher->UpdateTransformation();
        SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
        KG3DSceneEntity Entity(SCENEENTITY_SFX_LAUNCHER, pLauncher, pData);
        m_vecLauncherEntityData.push_back(pData);
        m_listRenderEntity.push_back(Entity);
        
		//m_listSelectableEntity.push_back(Entity);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);
    }

    return hr;
}

HRESULT KG3DSceneSFXEditor::RemoveLauncher(DWORD dwIndex)
{
    KG3DSFXParticleLauncher *pLauncher = NULL;
    if (FAILED(m_lpSFX->GetParticleLauncher(dwIndex, &pLauncher)))
        return E_FAIL;

    KG3DSceneEntity Entity(SCENEENTITY_SFX_LAUNCHER, pLauncher);
    /*if (FAILED(RemoveSelectableEntity(Entity)))
        return E_FAIL;*/
	HRESULT hr = this->GetSelectionTool().SetSelectable(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);
	hr = this->GetSelectionTool().SetSelected(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);

    m_listRenderEntity.remove(Entity);
    //m_listSelectedEntity.remove(Entity);
	this->GetSelectionTool().SetSelected(Entity, FALSE);

    if (m_vecLauncherEntityData.size() > dwIndex)
    {
        SAFE_DELETE(m_vecLauncherEntityData[dwIndex]);
        m_vecLauncherEntityData.erase(m_vecLauncherEntityData.begin() + dwIndex);
    }

    return m_lpSFX->RemoveParticleLauncher(dwIndex);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSFXEditor::RemoveBillboard(DWORD dwIndex)
{
    KG3DSFXBillboard *pBillboard = NULL;
    if (FAILED(m_lpSFX->GetBillboard(dwIndex, &pBillboard)))
        return E_FAIL;

    KG3DSceneEntity Entity(SCENEENTITY_SFX_BILLBOARD, pBillboard);
   /* if (FAILED(RemoveSelectableEntity(Entity)))
        return E_FAIL;*/
	HRESULT hr = this->GetSelectionTool().SetSelectable(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);
	hr = this->GetSelectionTool().SetSelected(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);

    //m_listSelectedEntity.remove(Entity);
	this->GetSelectionTool().SetSelected(Entity, FALSE);
    
    if (m_vecBillboardEntityData.size() > dwIndex)
    {
        SAFE_DELETE(m_vecBillboardEntityData[dwIndex]);
        m_vecBillboardEntityData.erase(m_vecBillboardEntityData.begin() + dwIndex);
    }

    return m_lpSFX->RemoveBillboard(dwIndex);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSFXEditor::RemoveTrack(DWORD dwIndex)
{
    KG3DSFXTrack *pTrack = NULL;
    if (FAILED(m_lpSFX->GetTrack(dwIndex, &pTrack)))
        return E_FAIL;

    KG3DSceneEntity Entity(SCENEENTITY_SFX_TRACK, pTrack);
   /* if (FAILED(RemoveSelectableEntity(Entity)))
        return E_FAIL;*/
	HRESULT hr = this->GetSelectionTool().SetSelectable(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);
	hr = this->GetSelectionTool().SetSelected(Entity, FALSE);
	KG_COM_PROCESS_ERROR(hr);

    m_listRenderEntity.remove(Entity);
    //m_listSelectedEntity.remove(Entity);
	this->GetSelectionTool().SetSelected(Entity, FALSE);

    SAFE_DELETE(m_vecTrackEntityData[dwIndex]);
    m_vecTrackEntityData.erase(m_vecTrackEntityData.begin() + dwIndex);

    m_vecTrackBindInformation.erase(m_vecTrackBindInformation.begin() + dwIndex);

    return m_lpSFX->RemoveTrack(dwIndex);
Exit0:
	return E_FAIL;
}

void KG3DSceneSFXEditor::MsgOnRButtonUp(WPARAM wParam, LPARAM lParam)
{
    KG3DSceneEditorBase::MsgOnRButtonUp(wParam, lParam);

	if (!IsModifyMode())
	{
		//m_pCurBindModel  = NULL;
		m_pCurBeBindMdl  = NULL;
		m_nLinkingFlag   = BIND_STATE_FREE;
	}
}

//<Move Refactor>
//HRESULT KG3DSceneSFXEditor::MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
//{
//    this->GetCoordMove().MoveBegin();
//     m_bMoving = TRUE;
//
//    return S_OK;
//}
//<Move Refactor>
//HRESULT KG3DSceneSFXEditor::MoveEnd()
//{
//	m_bMoving = FALSE;
//	this->GetCoordMove().MoveEnd();
////Exit0:
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::MoveFrameMove(/*KG3DCamera* pCamera,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY*/)//<Move Refactor>参数无效
//{
//    this->GetCoordMove().FrameMove();
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::OnToggleStateMove()	
//{        
//    m_bMoving = FALSE;
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::GetSelectedEntityCenter(D3DXVECTOR3* pPos)
//{
//    if (m_listSelectedEntity.size() == 0)
//        return E_FAIL;
//    
//    KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
//    while (i != m_listSelectedEntity.end())
//    {
//        KG3DSceneEntity Entity = *i;
//        AABBOX Box;
//        Entity.GetBBox(&Box);
//
//        if (i == m_listSelectedEntity.begin())
//            m_SelectionCenter = Box.GetCenter();
//        else
//            m_SelectionCenter = (m_SelectionCenter + Box.GetCenter()) / 2.f;
//        i++;
//    }
//
//    if(pPos)
//    {
//        (*pPos) = m_SelectionCenter;
//    }
//
//    return S_OK;
//}

/*	//2009-1-15 这个和SelectBase 里面的同名函数除了开头一点点，基本完全一致，那么把开头的一点移动到RenderMainWindow好了
HRESULT KG3DSceneSFXEditor::RenderSelectedEntityMark(DWORD dwEditState,D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View,BOOL bShowScreenMask)
{
    if (this->GetEditState() == SCENESTATE_SFX_GEO)
        return S_OK;

	if(0 != GetSelectionCount())
	{
		D3DXVECTOR3 X(10,0,0);
		D3DXVECTOR3 Y(0,10,0);
		D3DXVECTOR3 Z(0,0,10);
		D3DXVECTOR3 vCenter = KG3DSceneSelectBase::GetCenterPos();
		g_cGraphicsTool.DrawLine(&(vCenter-X),&(vCenter+X),0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&(vCenter-Y),&(vCenter+Y),0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&(vCenter-Z),&(vCenter+Z),0xFF00FF00,0xFF00FF00);
	}

    GraphicsStruct::RenderState zenable(D3DRS_ZENABLE,FALSE);

    if((m_bSelecting)&&(bShowScreenMask))
    {
        GraphicsStruct::RenderState zWrite(D3DRS_ZWRITEENABLE,FALSE);
        GraphicsStruct::RenderStateAlpha Apha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

        g_pd3dDevice->SetTexture(0, NULL);
        g_cGraphicsTool.DrawScreenRect(&m_Selection_Origin2D_A,&m_Selection_Origin2D_C,0,0x30FFFFFF);

    }

    RenderSelectableEntityMark(Viewport,Projection,View);

    if(m_bSelecting && dwEditState == SCENESTATE_SELECT)
    {
        KG3DSceneEntityList::iterator i = m_listTempSelectedEntity.begin();
        while (i != m_listTempSelectedEntity.end())
        {
            KG3DSceneEntity Entity = *i;
            Entity.RenderSelectMark(0xFF00FF00);
            i++;
        }
    }
    else if (dwEditState == SCENESTATE_SELECT)
    {
		TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
		for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		{
			KG3DSceneEntity Entity = proxy.GetCur();
            Entity.RenderSelectMark(0xFFFF0000);
        }
    }

    DWORD dwLight;
    g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    BOOL LightEnable[8];
    for(int i=0;i<8;i++)
    {
        g_pd3dDevice->GetLightEnable(i,&LightEnable[i]);
        g_pd3dDevice->LightEnable(i,FALSE);
    }
   
	if (0 == this->GetSelectionTool().GetSelectionCount())
    {
        return E_FAIL;
    }
    switch(dwEditState)
    {
    case SCENESTATE_MOVE:
        this->GetCoordMove().Render();
        break;
    case SCENESTATE_ROTATE:
        this->GetCoordRotate().Render();
        break;
    case SCENESTATE_SCALE:
        this->GetCoordScale().Render();
    }
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
    for(int i=0;i<8;i++)
    {
        g_pd3dDevice->LightEnable(i,LightEnable[i]);
    }

    return S_OK;
}
*/

//HRESULT KG3DSceneSFXEditor::OnToggleStateRotation()
//{
//    //this->GetCoordRotate().AttachEntity(this, m_listSelectedEntity);
//    m_bRotating = FALSE;
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
//{
//    m_bRotating = TRUE;
//    this->GetCoordRotate().RotateBegin();
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::RotateEnd()
//{
//    m_bRotating = FALSE;
//    this->GetCoordRotate().RotateEnd();
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::RotateFrameMove(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction,float XX,float YY,float ZZ)
//{
//    this->GetCoordRotate().FrameMove();
//
//    return S_OK;
//}

//
//HRESULT KG3DSceneSFXEditor::OnToggleStateScaling()
//{
//    //this->GetCoordScale().AttachEntity(this, m_listSelectedEntity);
//    m_bScaling = FALSE;
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
//{
//    m_bScaling = TRUE;
//    this->GetCoordScale().ScalBegin();
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::ScaleEnd()
//{
//    m_bScaling = FALSE;
//    this->GetCoordScale().ScalEnd();
//
//    return S_OK;
//}

//HRESULT KG3DSceneSFXEditor::ScaleFrameMove(KG3DCamera* pCamera, D3DXVECTOR3& Origin, D3DXVECTOR3& Direction, float XX, float YY, float ZZ )
//{
//    this->GetCoordScale().FrameMove();
//
//    return S_OK;
//}


void KG3DSceneSFXEditor::SetCoordMode(DWORD dwMode) 
{
    this->GetCoordMove().SetMode(dwMode);
    this->GetCoordScale().SetMode(dwMode);
    this->GetCoordRotate().SetMode(dwMode);
}


void KG3DSceneSFXEditor::MsgOnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    int nResult  = false;

    KG3DSceneEntity Entity;
    KG3DModel*      pSelModel = NULL;

    KG3DSceneEditorBase::MsgOnLButtonUp(wParam, lParam);
    KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/0 != this->GetSelectionTool().GetSelectionCount());

    if (m_bIsModify)
    {
        BindSelMdlToTargetMdl();
    }
    else
    {

        //Entity = *(m_listSelectedEntity.begin());
		Entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();
        KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);

        pSelModel = (KG3DModel*)Entity.m_pPointer;
        KG_PROCESS_ERROR(pSelModel);

        if (find(m_vecRefModels.begin(), m_vecRefModels.end(), pSelModel) == m_vecRefModels.end())
            return;

        KG_PROCESS_ERROR(m_dwSceneMode == SFX_SCENE_MODE_BIND);
        
        switch (m_nLinkingFlag)
        {
        case BIND_STATE_FREE :
            m_pCurBeBindMdl = pSelModel;
            m_nLinkingFlag = BIND_STATE_LINKING;
            break;

        case BIND_STATE_LINKING :
            KG_PROCESS_ERROR(pSelModel != m_pCurBeBindMdl);
            m_pCurBindModel = pSelModel;
            BindSelMdlToTargetMdl();
            //m_pCurBindModel  = NULL;
            //m_pCurBeBindMdl  = NULL;
            //m_nLinkingFlag = BIND_STATE_FREE;
            break;
        default :
            ASSERT(0);
            break;
        }
    }

    nResult = true;
Exit0:
    return;
}

void KG3DSceneSFXEditor::MsgOnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    KG3DSceneEditorBase::MsgOnLButtonDown(wParam, lParam);

    if (m_SceneWndManager->GetCurOutputWnd())
    {	
        D3DXVECTOR3 RayOrig,RayDir;
        D3DXVECTOR2 RayOrig2D;
        m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&RayOrig,&RayDir,&RayOrig2D);
        float x = RayOrig.x - (RayOrig.y * RayDir.x) / RayDir.y;
        float y = 0;
        float z = RayOrig.z - (RayOrig.y * RayDir.z) / RayDir.y;

        if (this->GetEditState() == SCENESTATE_SFX_ADD_LAUNCHER)
            AddLauncher(x, y, z);
        else if (this->GetEditState() == SCENESTATE_SFX_ADD_BILLBOARD)
            AddBillboard(x, y, z);
    }


    if (this->GetEditState() == SCENESTATE_COPY && m_lpSFX)
    {
        size_t uBillBoardNum = m_lpSFX->GetBillboardNum();
        size_t uBillBoardEntityDataNum = m_vecBillboardEntityData.size();

        for (size_t i = uBillBoardEntityDataNum; i < uBillBoardNum; i++)
        {
            SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
            m_vecBillboardEntityData.push_back(pData);
        }
    }
}

void KG3DSceneSFXEditor::MsgOnKeyDown(WPARAM wParam, LPARAM lParam)
{
    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
    
    switch(wParam)
    {
    case VK_F4 :
        m_bRenderLauncherFlag = !m_bRenderLauncherFlag;
        break;
    case 'L':
    case 'l':
         SetEditState(SCENESTATE_SFX_ADD_LAUNCHER);
        break;

    case 'B':
    case 'b':
        //SetEditState(m_dwEditState = SCENESTATE_SFX_ADD_BILLBOARD);
		KG3DSceneEditorBase::SetEditState(SCENESTATE_SFX_ADD_BILLBOARD);//注意，这里的意思是避开复写的SetEditState里面的ReleaseGeoElement
        break;

    case VK_OEM_PLUS :
    case VK_ADD :
		switch (this->GetEditState())
        {
        case SCENESTATE_MOVE :
            this->GetCoordMove().Zoom(0.1f);
            break;
        case SCENESTATE_ROTATE :
            this->GetCoordRotate().Zoom(0.1f);
            break;
        case SCENESTATE_SCALE :
            this->GetCoordScale().Zoom(0.1f);
            break;
        default :
            break;
        }
        break;

    case VK_OEM_MINUS :
    case VK_SUBTRACT :
        switch (this->GetEditState())
        {
        case SCENESTATE_MOVE :
            this->GetCoordMove().Zoom(-0.1f);
            break;
        case SCENESTATE_ROTATE :
            this->GetCoordRotate().Zoom(-0.1f);
            break;
        case SCENESTATE_SCALE :
            this->GetCoordScale().Zoom(-0.1f);
            break;
        default :
            break;
        }
        break;

    case VK_DELETE:
        {
            if (m_dwSceneMode == SFX_SCENE_MODE_BIND)
                break;

            while (/*!m_listSelectedEntity.empty()*/0 != this->GetSelectionTool().GetSelectionCount())
            {
                //KG3DSceneEntity Entity = *m_listSelectedEntity.begin();
				KG3DSceneEntity Entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();
                RemoveSFXEntity(Entity);
            }
        }
        break;

    case 'Q':
        SetEditState(SCENESTATE_SELECT);
        m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
        piCameraAnimation->StopPlay();
        break;

    case 'W':
        {
            if (/*m_listSelectedEntity.size() == 0*/0 == this->GetSelectionTool().GetSelectionCount() )
                break;

            SetEditState(SCENESTATE_MOVE);
            //OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态

            //if(m_SceneWndManager->GetCurOutputWnd())	//2009-1-22 去掉，内部会自动响应
            //{
            //    D3DXVECTOR3 RayOrig,RayDir;
            //    m_SceneWndManager->GetCurOutputWnd()->GetPickRay(&RayOrig,&RayDir, NULL);
            //    MoveFrameMove(/*&m_SceneWndManager->GetCurOutputWnd()->GetCamera(),RayOrig,RayDir,0,0*/);	
            //}
        }
        break;

    case 'E':
        {
            if (this->GetSelectionTool().GetSelectionCount() == 0)
                break;

            SetEditState(SCENESTATE_ROTATE);
            //OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
        }
        break;
    case 'R':
        {
            SetEditState(SCENESTATE_SCALE);
            //OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
            if (/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount() == 0)
                break;
        }
        break;

    default:
        break;
    }

    KG3DSceneEditorBase::MsgOnKeyDown(wParam,lParam);

    if ((wParam == 'v' || wParam == 'V') && m_lpSFX)
    {
        size_t uBillBoardNum = m_lpSFX->GetBillboardNum();
        size_t uBillBoardEntityDataNum = m_vecBillboardEntityData.size();

        for (size_t i = uBillBoardEntityDataNum; i < uBillBoardNum; i++)
        {
            SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
            m_vecBillboardEntityData.push_back(pData);
        }

        //
        //KG3DSceneEntity Entity(SCENEENTITY_SFX_BILLBOARD, pBillboard, pData);
        //m_vecBillboardEntityData.push_back(pData);
    }

}

HRESULT KG3DSceneSFXEditor::LoadRefrenceModel(LPSTR pszMesh)
{

    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;


    if (m_bIsModify)
    {
        ASSERT(m_vecRefModels.size() == 2);

        KG3DModel* pModel = NULL;
        if (FAILED(hRetCode = g_cModelTable.LoadResourceFromFile(pszMesh, 0, MLO_MULTI, (IKG3DResourceBase**)&pModel)))
        {
            SAFE_RELEASE(pModel);
            KG_PROCESS_ERROR(false);
        }
		if (pModel->GetType() == MESHTYPE_SFX)
		{
			KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(pModel);
			pSFX->SetMeteorSampling(TRUE);
		}
        KG3DSceneEntity EntityDel(SCENEENTITY_MODEL, m_vecRefModels[1]);

        m_listRenderEntity.remove(EntityDel);
        
		//m_listSelectableEntity.remove(EntityDel);
		this->GetSelectionTool().SetSelectable(EntityDel, FALSE);

        m_listPostRenderEntity.remove(EntityDel);
        //m_listSelectedEntity.remove(EntityDel);
		this->GetSelectionTool().SetSelected(EntityDel, FALSE);

        SAFE_RELEASE(m_vecRefModels[1]);
        
        m_vecRefModels[1] = pModel;

        KG3DSceneEntity EntityAdd(SCENEENTITY_MODEL, m_vecRefModels[1]);

        m_listRenderEntity.push_back(EntityAdd);
        
		//m_listSelectableEntity.push_back(EntityAdd);
		this->GetSelectionTool().SetSelectable(EntityAdd, TRUE);

        ASSERT(m_pModifySuede);

        hRetCode = m_pModifySuede->ModifyInfo(
            m_nModifyIndex, 
            pModel,
            m_pModifySuede->m_vecBindInfo[m_nModifyIndex].dwBindType,
            m_pModifySuede->m_vecBindInfo[m_nModifyIndex].strTarName
            );
        KG_COM_PROCESS_ERROR(hRetCode);

    }
    else
    {
       hRetCode = LoadHandModel(pszMesh);
       KG_COM_PROCESS_ERROR(hRetCode);

    }
	
    hResult = S_OK;
Exit0:
    return hResult;


 
}

//KG3DSFXSuede* KG3DSceneSFXEditor::GetSFXSuede()
//{
////    return m_pSFXSuede;
//}

HRESULT KG3DSceneSFXEditor::GetSFXSceneMode(DWORD *pdwRetSFXSceneMode)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetSFXSceneMode);

    *pdwRetSFXSceneMode = m_dwSceneMode;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::UpdateSFXBindIndex(int nIndex)
{
    //ASSERT(nIndex <= (int)m_pSFXSuede->GetSFXCount());
    //ASSERT(nIndex != -1);
    m_nCurBindSfxIndex = nIndex;
    return S_OK;
}

int KG3DSceneSFXEditor::GetCurSfxBindIndex()
{
    return m_nCurBindSfxIndex;
}

//------------------------------------------------------------------------------
// Name : UpdateSFXSuedeSFX
// Desc : 
//------------------------------------------------------------------------------
HRESULT KG3DSceneSFXEditor::UpdateSFXSuedeSFX(LPSTR pszPath)
{
    HRESULT hResult  = E_FAIL;

   /* KG3DModel* pOldSFX = NULL;
    KG3DSFX* pSFX = NULL;
    KG_PROCESS_ERROR(m_pSFXSuede);

    ASSERT(m_nCurBindSfxIndex < (int)m_pSFXSuede->GetSFXCount());
    g_cModelTable.LoadResourceFromFile( pszPath, 0, MLO_MULTI, (IKG3DResourceBase**)&pSFX);
    KG_PROCESS_ERROR(pSFX);
    pOldSFX = m_pSFXSuede->UpdateSFX(pSFX, (DWORD)m_nCurBindSfxIndex);

    {
        KG3DSceneEntity entityDel(SCENEENTITY_MODEL, pOldSFX);
        m_listPostRenderEntity.remove(entityDel);
        m_listRenderEntity.remove(entityDel);
        m_listSelectableEntity.remove(entityDel);
        m_listSelectedEntity.remove(entityDel);
        SAFE_RELEASE(pOldSFX);   

        KG3DSceneEntity entityAdd(SCENEENTITY_MODEL, pSFX);
        if (pSFX->m_nIsSockWave)
            m_listPostRenderEntity.push_back(entityAdd);
        else
            m_listRenderEntity.push_back(entityAdd); 
        m_listSelectableEntity.push_back(entityAdd);
    }*/

    hResult = S_OK;
//Exit0:
    return hResult;
}

//------------------------------------------------------------------------------
// Name : UpdateSceneEntity
// Desc : 
//------------------------------------------------------------------------------
HRESULT KG3DSceneSFXEditor::UpdateSceneEntity()
{
    HRESULT hResult  = E_FAIL;

//    KG_PROCESS_ERROR(m_pSFXSuede);


    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::BindSelMdlToTargetMdl()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    TCHAR   szTargetName[128];
    DWORD   dwTargetType;

    if (m_bIsModify)
    {
        ASSERT(m_vecRefModels.size() == 2);

        m_pCurBindModel = m_vecRefModels[0];
        m_pCurBeBindMdl = m_vecRefModels[1];
    }

    KG_PROCESS_ERROR(m_pCurBeBindMdl != m_pCurBindModel);
    KG_PROCESS_ERROR(m_pCurBindModel);
    KG_PROCESS_ERROR(m_pCurBeBindMdl);

    m_pCurBindModel->GetBoneMesh()->GetSelBoneName(
        szTargetName, sizeof(szTargetName), &dwTargetType
    );

    if (m_bIsModify)
    {
        if (dwTargetType == KG3DSFXSuede::BIND_TYPE_ORG)
            return S_OK;
    }


    hRetCode = BindSelMdlToTargetMdl(szTargetName, dwTargetType);
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::BindSelMdlToTargetMdl(const TCHAR szName[], DWORD dwType)
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DModel*     pParentModle = NULL;
    D3DXVECTOR3    vNorTrans = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXQUATERNION qNorRotation;

    if (m_bIsModify)
    {
        ASSERT(m_vecRefModels.size() == 2);

        m_pCurBindModel = m_vecRefModels[0];
        m_pCurBeBindMdl = m_vecRefModels[1];
    }

    KG_PROCESS_ERROR(m_pCurBeBindMdl != m_pCurBindModel);
    KG_PROCESS_ERROR(m_pCurBindModel);
    KG_PROCESS_ERROR(m_pCurBeBindMdl);

    m_pCurBindModel->GetParentModel((IKG3DModel**)pParentModle);
    KG_PROCESS_ERROR(pParentModle!= m_pCurBeBindMdl);

    D3DXQuaternionRotationYawPitchRoll(&qNorRotation, 0.0f, 0.0f, 0.0f);

    switch (dwType)
    {
    case KG3DSFXSuede::BIND_TYPE_ORG :    

        hRetCode = m_pCurBeBindMdl->UnBindFromOther();
        KG_COM_PROCESS_ERROR(hRetCode);
        m_pCurBeBindMdl->SetTranslation(&vNorTrans);
        m_pCurBeBindMdl->SetRotation(&qNorRotation);
        m_pCurBeBindMdl->BindToBone(m_pCurBindModel, TEXT(""));

        if (m_bIsModify)
        {
            ASSERT(m_pModifySuede);
            hRetCode = m_pModifySuede->ModifyInfo(
                m_nModifyIndex, 
                m_pCurBeBindMdl,
                KG3DSFXSuede::BIND_TYPE_ORG,
                TEXT("")
                );
            if (FAILED(hRetCode))
            {
                m_nLinkingFlag = BIND_STATE_FREE;
                KG_PROCESS_ERROR(false);
            }

        }

        break;
    case KG3DSFXSuede::BIND_TYPE_BONE :

        hRetCode = m_pCurBeBindMdl->UnBindFromOther();
        KG_COM_PROCESS_ERROR(hRetCode);
        m_pCurBeBindMdl->SetTranslation(&vNorTrans);
        m_pCurBeBindMdl->SetRotation(&qNorRotation);
        m_pCurBeBindMdl->BindToBone(m_pCurBindModel, szName);

        if (m_bIsModify)
        {
            ASSERT(m_pModifySuede);
            hRetCode = m_pModifySuede->ModifyInfo(
                m_nModifyIndex, 
                m_pCurBeBindMdl,
                KG3DSFXSuede::BIND_TYPE_BONE,
                szName
                );
            if (FAILED(hRetCode))
            {
                m_nLinkingFlag = BIND_STATE_FREE;
                KG_PROCESS_ERROR(false);
            }
        }

        break;
    case KG3DSFXSuede::BIND_TYPE_SOCKET :

        hRetCode = m_pCurBeBindMdl->UnBindFromOther();
        KG_COM_PROCESS_ERROR(hRetCode);
        m_pCurBeBindMdl->SetTranslation(&vNorTrans);
        m_pCurBeBindMdl->SetRotation(&qNorRotation);
        m_pCurBeBindMdl->BindToSocket(m_pCurBindModel, szName);

        if (m_bIsModify)
        {
            ASSERT(m_pModifySuede);
            hRetCode = m_pModifySuede->ModifyInfo(
                m_nModifyIndex, 
                m_pCurBeBindMdl,
                KG3DSFXSuede::BIND_TYPE_SOCKET,
                szName
                );
            if (FAILED(hRetCode))
            {
                m_nLinkingFlag = BIND_STATE_FREE;
                KG_PROCESS_ERROR(false);
            }
        }

        break;
    default :
        break;
    }

    m_nLinkingFlag = BIND_STATE_FREE;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::UnBindSelModel()
{
    HRESULT hResult  = E_FAIL;

    KG3DSceneEntity Entity;
    KG3DModel*   pSelModel = NULL;

	KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount());

	/*Entity = *(m_listSelectedEntity.begin());*/Entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();
    KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);

    pSelModel = (KG3DModel*)Entity.m_pPointer;
    KG_PROCESS_ERROR(pSelModel);

    pSelModel->UnBindFromOther();

    hResult = S_OK;
Exit0:
    return hResult;
}


HRESULT KG3DSceneSFXEditor::ExportSelModelSuede(const TCHAR szFileName[])
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DSFXSuede*   pSFXSuede = new KG3DSFXSuede();    
    KG3DSceneEntity Entity;
    KG3DModel*      pSelModel = NULL;

    KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount());

    //Entity = *(m_listSelectedEntity.begin());
	Entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();
    KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);

    pSelModel = (KG3DModel*)Entity.m_pPointer;
    
    if (pSelModel)
    {
        vector<KG3DModel*> vecChilds = pSelModel->GetAllChilds();
        vector<KG3DModel*>::iterator i = vecChilds.begin();
        while (i != vecChilds.end())
        {
            //if ((*i)->GetType() == MESHTYPE_SFX)
            {
                pSFXSuede->AddModel(reinterpret_cast<KG3DModel*>(*i));
                (*i)->AddRef();
            }
            i++;
        }
    }
    else
    {
        KG_PROCESS_ERROR(FALSE);
    }

    hRetCode = pSFXSuede->SaveToFile(szFileName);
    KG_COM_PROCESS_ERROR(hRetCode);

    hResult = S_OK;
Exit0:
    SAFE_DELETE(pSFXSuede);
    return hResult;
}

//
//  Import The Bind Information From The Bind File And Bind To The Selected Model
//

HRESULT KG3DSceneSFXEditor::ImportSuedeToSelMdl(const TCHAR szFileName[])
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    KG3DSFXSuede*   pSFXSuede = new KG3DSFXSuede();
    KG3DSceneEntity Entity;
    KG3DModel*      pSelModel = NULL;

    KG_PROCESS_ERROR(/*m_listSelectedEntity.size()*/this->GetSelectionTool().GetSelectionCount());

    //Entity = *(m_listSelectedEntity.begin());
	Entity = this->GetSelectionTool().GetSelectionAccessProxy().GetFront();
    KG_PROCESS_ERROR(Entity.GetType() == SCENEENTITY_MODEL);

    pSelModel = (KG3DModel*)Entity.m_pPointer;

    DWORD dwCount = 0;

    //hRetCode = pSFXSuede->LoadFromBindFile(pSelModel, szFileName);
    hRetCode = pSFXSuede->LoadFromFile(szFileName, 0, 0);
    KG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = pSFXSuede->Bind(pSelModel);
    KG_COM_PROCESS_ERROR(hRetCode);
    dwCount = pSFXSuede->GetModelCount();

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
    for (DWORD i = 0; i < dwCount; i++)
    {
        IKG3DModel* pSFX = pSFXSuede->GetModel(i);
        if (pSFX)
        {
            m_vecRefModels.push_back((KG3DModel*)pSFX);
            KG3DSceneEntity e(SCENEENTITY_MODEL, pSFX);
            if (((KG3DModel*)pSFX)->m_nIsSockWave)
            {
                m_listPostRenderEntity.push_back(e);
                if (((KG3DModel*)pSFX)->m_dwShockMode == SHOCK_WAVE_ADD)
                    m_listRenderEntity.push_back(e);
            }
            else
                m_listRenderEntity.push_back(e);
            
			//m_listSelectableEntity.push_back(e);
			selTool.SetSelectable(e, TRUE);
            
			pSFX->AddRef();
			//打开刀光特效取样////////////////////////////////////////////
			if (pSFX->GetType() == MESHTYPE_SFX)
			{
				static_cast<KG3DSFX*>(pSFX)->SetMeteorSampling(TRUE);		
			}
        }
    }   

    hResult = S_OK;
Exit0:
    SAFE_DELETE(pSFXSuede);
    return hResult;
}
//------------------------------------------------------------------------------
// Name : BindSuedeSFX
// Desc : 
//------------------------------------------------------------------------------
HRESULT KG3DSceneSFXEditor::BindSuedeSFX()
{
    HRESULT hResult  = E_FAIL;

    //KG3DModel


    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::BindSuedeSFX(LPSTR pszBoneName)
{
    HRESULT hResult  = E_FAIL;

 /*   KG3DSFX* pModel  = NULL;

    KG_PROCESS_ERROR(pszBoneName);
    KG_PROCESS_ERROR(m_pSFXSuede);
    KG_PROCESS_ERROR(m_pBindModel);

    ASSERT(m_nCurBindSfxIndex < (int)m_pSFXSuede->GetSFXCount());
    KG_PROCESS_ERROR(m_nCurBindSfxIndex < (int)m_pSFXSuede->GetSFXCount());

    pModel = dynamic_cast<KG3DSFX*>(m_pSFXSuede->GetSFX(m_nCurBindSfxIndex));
    KG_PROCESS_ERROR(pModel);

    hRetCode = pModel->BindToBone(m_pBindModel, pszBoneName);
    KG_COM_PROCESS_ERROR(hRetCode);*/

    /*{
        KG3DSceneEntity entity(SCENEENTITY_MODEL, pModel);
        m_listRenderEntity.push_back(entity);
        m_listSelectableEntity.push_back(entity);
    }*/

    hResult = S_OK;
//Exit0:
    return hResult;
}

//------------------------------------------------------------------------------
// Name : UnBindSuedeSFX
// Desc : 
//------------------------------------------------------------------------------
HRESULT KG3DSceneSFXEditor::UnBindSuedeSFX()
{
    HRESULT hResult  = E_FAIL;

    //KG3DModel* pBindModel = NULL;
    //KG3DSFX*   pModel     = NULL;

    //string strBoneName;
    //int    nBoneIdx = -1;
   
    //KG_PROCESS_ERROR(m_pSFXSuede);
    //ASSERT(m_nCurBindSfxIndex < (int)m_pSFXSuede->GetSFXCount());
    //KG_PROCESS_ERROR(m_nCurBindSfxIndex < (int)m_pSFXSuede->GetSFXCount());

    //pModel = dynamic_cast<KG3DSFX*>(m_pSFXSuede->GetSFX(m_nCurBindSfxIndex));
    //KG_PROCESS_ERROR(pModel);
    //KG_PROCESS_SUCCESS(pModel->GetBindType() != ENUM_BIND_BONE);

    ////
    ////pModel->GetParentModel((IKG3DModel**)&pBindModel);
    ////
    //pModel->GetBindModel((IKG3DModel**)&pBindModel);
    //KG_PROCESS_ERROR(pBindModel);
    //ASSERT(pBindModel == m_pBindModel);

    //hRetCode = pModel->GetBindBoneName(strBoneName);
    //KG_COM_PROCESS_ERROR(hRetCode);

    //nBoneIdx = pBindModel->FindBone(strBoneName.c_str(), NULL);
    //if (nBoneIdx != -1)
    //{
    //    D3DXMATRIX matBone;
    //    hRetCode = pBindModel->GetBoneMatrix(nBoneIdx, matBone, NULL);
    //    KG_COM_PROCESS_ERROR(hRetCode);

    //    //pModel->m_Matrix *= matBone;
    //    pModel->ReSet();
    //    pModel->UnBindFromOther();
    //}
    //else
    //    pModel->UnBindFromOther();

    //{
    //    KG3DSceneEntity entity(SCENEENTITY_MODEL, pModel);
    //    if (pModel->m_nIsSockWave)
    //        m_listPostRenderEntity.remove(entity);
    //    else
    //        m_listRenderEntity.remove(entity);
    //    m_listSelectableEntity.remove(entity);
    //    m_listSelectedEntity.remove(entity);
    //}

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}


HRESULT KG3DSceneSFXEditor::LoadBindModel(LPSTR pszMesh)
{ 
    ASSERT(pszMesh);
//    KG3DModel *pModel = NULL;
    HRESULT hRet = E_FAIL;
//	HRESULT hr = g_cModelTable.LoadResourceFromFile(
//        pszMesh, 0, MLO_MULTI, (IKG3DResourceBase **)&pModel
//    );
//	KG_COM_PROCESS_ERROR(hr);
//
//    hRet = S_OK;
//Exit0:
//    if (hRet == S_OK)
//    {
//        if (m_pBindModel)
//        {
//            KG3DSceneEntity Entity1(SCENEENTITY_MODEL,m_pBindModel);
//            if (m_pBindModel->m_nIsSockWave)
//                m_listPostRenderEntity.remove(Entity1);
//            else
//                m_listRenderEntity.remove(Entity1);
//            m_listSelectableEntity.remove(Entity1);
//            m_listSelectedEntity.remove(Entity1);
//        }
//        //RemoveSelectableEntity(Entity1);
//        //RemoveSelectedEntity(Entity1);
//
//        //if (m_pSFXModel)
//        //{
//        //    m_pSFXModel->UnBindFromOther();
//        //    // do this 
//        //    /*KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pSFXModel);
//        //    m_listRenderEntity.remove(Entity);
//        //    m_listSelectableEntity.remove(Entity);
//        //    m_listSelectedEntity.remove(Entity);
//        //    SAFE_RELEASE(m_pSFXModel);*/
//        //}
//
//        if (m_pBindModel)
//        {
//            //KG3DSFXSuede *psuede = (KG3DSFXSuede*)m_pBindModel->GetSFXSuede();
//            //if (psuede)
//            //{
//            //    for (DWORD i = 0; i < psuede->GetSFXCount(); ++i)
//            //    {
//            //        //psuede->GetSFX(i)->UnBindFromOther();
//
//            //        //
//            //        KG3DModel* pSFX = dynamic_cast<KG3DSFX*>(psuede->GetSFX(i));
//            //        if (pSFX)
//            //        {
//            //            pSFX->UnBindFromOther();
//            //            KG3DSceneEntity Entity(SCENEENTITY_MODEL, pSFX);
//            //            m_listRenderEntity.remove(Entity);
//            //            m_listSelectableEntity.remove(Entity);
//            //            m_listSelectedEntity.remove(Entity);   
//            //        }
//            //    }
//            //}
//        }
//
//
//        //
//        // 尽量按上一个模型的绑定方式绑定到当前模型
//        //
//        
//        if (m_pSFXSuede)
//        {
//            for (DWORD i = 0; i < m_pSFXSuede->GetSFXCount(); i++)
//            {
//                 KG3DModel* pSFX = dynamic_cast<KG3DSFX*>(m_pSFXSuede->GetSFX(i));
//                 if (pSFX && pSFX->GetBindType() == ENUM_BIND_BONE)
//                 {
//                    KG3DModel* pBindModel = NULL;
//                    string strBoneName;
//                    HRESULT hRetCode = pSFX->GetBindBoneName(strBoneName);
//                    ASSERT(hRetCode == S_OK);
//                    pSFX->UnBindFromOther();
//                    pSFX->BindToBone(pModel, strBoneName.c_str());
//                 }
//            }
//        }
//        SAFE_RELEASE(m_pBindModel);
//        m_pBindModel = pModel;
//
//        KG3DSceneEntity Entity2(SCENEENTITY_MODEL,m_pBindModel);
//        if (m_pBindModel->m_nIsSockWave)
//            m_listPostRenderEntity.push_back(Entity2);
//        else
//            m_listRenderEntity.push_back(Entity2); 
//        m_listSelectableEntity.push_back(Entity2);
//        m_listSelectedEntity.push_back(Entity2);
//    }
//    else
//        SAFE_RELEASE(pModel);
//
//    //m_meshBone.Create(m_pBindModel);

    return hRet;
}

HRESULT KG3DSceneSFXEditor::LoadBindModelAnimation(LPSTR pszName)
{
    ASSERT(pszName);
    if (!m_pBindModel)
        return E_FAIL;
	HRESULT hr = m_pBindModel->PlayAnimation(
		ENU_ANIMATIONPLAY_CIRCLE, pszName, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY
	);         
    return hr;
}
//
//HRESULT KG3DSceneSFXEditor::LoadRefrenceModelAnimation(LPSTR pszName)
//{
//    HRESULT hResult  = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//
//    IKG3DAnimationController* pAniController = NULL;
//    FLOAT  fSFXCurFrame  = 0;
//    DWORD  dwSFXPlayMode = 0;
//    DWORD  dwMDLPlayMode = 0;
//    DWORD  dwSFXFrameDly = 0;
//
//    ASSERT(pszName);
//    KG_PROCESS_ERROR(m_pRefrenceModel);
//    KG_PROCESS_ERROR(m_lpSFX);
//
//    fSFXCurFrame  = m_lpSFX->GetCurrentFrame();
//    dwMDLPlayMode = m_lpSFX->GetModelPlayMode();
//    dwSFXPlayMode = m_lpSFX->GetPlayAnimation();
//    dwSFXFrameDly = m_lpSFX->GetFrameInterval();
//
//    hRetCode = m_pRefrenceModel->PlayAnimation(
//        dwMDLPlayMode, pszName, 1.0f, 0, NULL
//    );        
//    KG_COM_PROCESS_ERROR(hRetCode);
//
//    pAniController = m_pRefrenceModel->GetAnimationController(ANICTL_PRIMARY);
//    KG_PROCESS_ERROR(pAniController);
//
//    pAniController->SeekAnimation(ANIMATION_SEEK_SET, fSFXCurFrame * dwSFXFrameDly);
//
//    if (dwSFXPlayMode == SFX_AP_PAUSE)
//        pAniController->PauseAnimation(TRUE);
//    else
//        pAniController->PauseAnimation(FALSE);
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}

HRESULT KG3DSceneSFXEditor::UnbindItemToSocket(LPSTR pszSocketName)
{
    HRESULT hResult  = E_FAIL;

    DWORD index = 0;

    KG_PROCESS_ERROR(pszSocketName);
    //KG_PROCESS_ERROR(m_pRefrenceModel);

    for (index= 0; index < m_vecITEM.size(); index++)
    {
        if (m_vecITEM[index].strSocketName == pszSocketName)
        {
            KG3DSceneEntity entityA(SCENEENTITY_MODEL, m_vecITEM[index].pItemModel);
            m_listRenderEntity.remove(entityA);
            SAFE_RELEASE(m_vecITEM[index].pItemModel);
            m_vecITEM.erase(m_vecITEM.begin() + index);
            break; 
        }
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::BindItemToSocket(LPSTR pszSocketName, LPSTR pszItemPath)
{
    HRESULT hResult  = E_FAIL;

    //KG3DModel* pModel = NULL;
    //DWORD index = 0;

    //KG_PROCESS_ERROR(pszSocketName);
    //KG_PROCESS_ERROR(pszItemPath);
    ////KG_PROCESS_ERROR(m_pRefrenceModel);

    //for (index= 0; index < m_vecITEM.size(); index++)
    //{
    //    if (m_vecITEM[index].strSocketName == pszSocketName)
    //        break; 
    //}
    //if (index == m_vecITEM.size())
    //{
    //    _BINDITEM bindItem;
    //    bindItem.strSocketName = pszSocketName;
    //    m_vecITEM.push_back(bindItem);  
    //}

    //hRetCode = g_cModelTable.LoadResourceFromFile(
    //    pszItemPath, 0, MLO_MULTI, (IKG3DResourceBase **)&pModel
    //);
    //KG_COM_PROCESS_ERROR(hRetCode);
    //ASSERT(pModel);
    //pModel->BindToSocket(m_pRefrenceModel, pszSocketName);
    //{
    //    KG3DSceneEntity entityA(SCENEENTITY_MODEL, m_vecITEM[index].pItemModel);
    //    m_listRenderEntity.remove(entityA);
    //    SAFE_RELEASE(m_vecITEM[index].pItemModel);
    //    m_vecITEM[index].pItemModel = pModel;
    //    KG3DSceneEntity entityB(SCENEENTITY_MODEL, m_vecITEM[index].pItemModel);
    //    m_listRenderEntity.push_back(entityB);
    //}

    hResult = S_OK;
//Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::EnableShowGround(int nEnable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_SUCCESS(nEnable && m_pGroundTexture);

    if (nEnable)
    {
        hr = g_cTextureTable.LoadResourceFromFile("data\\editor\\Texture\\ground.tga", 0, 0, (IKG3DResourceBase**)&m_pGroundTexture);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        SAFE_RELEASE(m_pGroundTexture);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_pGroundTexture);
    }
    return hrResult;
}

HRESULT KG3DSceneSFXEditor::EnableSoceWave(int nEnagle)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwSfxModelCount = 0;
    KG_PROCESS_ERROR(m_lpSFX);
    
    dwSfxModelCount = m_lpSFX->GetNumModels();

    if (nEnagle)
    {
        KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX);
        m_listRenderEntity.remove(entity);

        for (DWORD i = 0; i < dwSfxModelCount; i++)
        {
            KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX->GetModel(i));
            m_listRenderEntity.remove(entity);
        }

        m_listPostRenderEntity.push_back(entity);
    }
    else
    {
        KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX);
        m_listPostRenderEntity.remove(entity);

        for (DWORD i = 0; i < dwSfxModelCount; i++)
        {
            KG3DModel* pModel = m_lpSFX->GetModel(i);
            if (pModel)
            {
                KG3DSceneEntity entity(SCENEENTITY_MODEL, pModel);
                m_listRenderEntity.push_back(entity);
            }
        }

        m_listRenderEntity.push_back(entity);
    }
    m_lpSFX->m_nIsSockWave = nEnagle;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::SetShockMode(DWORD mode)
{
    HRESULT hResult  = E_FAIL;

    DWORD dwSfxModelCount = 0;

    KG_PROCESS_ERROR(m_lpSFX);
    KG_PROCESS_ERROR(m_lpSFX->m_nIsSockWave);

    if (mode == SHOCK_WAVE_ADD)
    {
        KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX);
        m_listRenderEntity.push_back(entity);

        for (DWORD i = 0; i < dwSfxModelCount; i++)
        {
            KG3DSceneEntity entity(SCENEENTITY_MODEL, m_lpSFX->GetModel(i));
            m_listRenderEntity.push_back(entity);
        }
    }

    m_lpSFX->m_dwShockMode = mode;

    hResult = S_OK;
Exit0:
    return hResult;
}

//HRESULT KG3DSceneSFXEditor::AddBindSFXModel()
//{
//    if (m_pSFXModel && m_pBindModel)
//    {
//        char szFileName[MAX_PATH];
//        KG3DModel *pModel = NULL;
//        HRESULT hr = g_cModelTable.Get1NewResource((KG3DModel**)&pModel, MESHTYPE_SFX, 0);
//        ASSERT(S_OK == hr);
//
//        hr = pModel->LoadFromFile(m_pSFXModel->m_scName.c_str(), 0, 0);
//        ASSERT(S_OK == hr);
//
//        pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, "", 1.0f, 0, 0);
//        KG3DSFXSuede *pSuede = (KG3DSFXSuede*)m_pBindModel->GetSFXSuede();
//        if (pSuede)
//        {
//            pSuede->AddSFX((KG3DSFX*)pModel);
//            return S_OK;
//        }
//    }
//
//    return E_FAIL;
//}

//HRESULT KG3DSceneSFXEditor::RemoveBindSFXModel(DWORD dwIndex)
//{
//    if (m_pBindModel)
//    {
//        KG3DSFXSuede *pSuede = (KG3DSFXSuede*)m_pBindModel->GetSFXSuede();
//        if (pSuede)
//        {
//            return pSuede->DelSFX(dwIndex);
//        }
//    }
//
//    return E_FAIL;
//}

HRESULT KG3DSceneSFXEditor::LoadMesh(LPSTR pszMeshFile)
{
    ASSERT(pszMeshFile);

    KG3DModel *pModel = NULL;
    HRESULT hr = g_cModelTable.Get1NewResource(&pModel, 0, 0);
    if (FAILED(hr))
        return E_FAIL;

    SAFE_RELEASE(m_pModel);
    m_pModel = pModel;

    hr = m_pModel->LoadMeshFromFile(pszMeshFile, 0);
    if (FAILED(hr))
        return E_FAIL;

    KG3DMesh *pMesh = m_pModel->GetMesh();
    DWORD dwNumBone = pMesh->m_dwNumBone;

    m_vecBindInformation.clear();
    for (DWORD i = 0; i < dwNumBone; ++i)
    {
        SFX_BIND_INFORMATION info;
        info.nLauncher1 = -1;
        info.nLauncher2 = -1;
        m_vecBindInformation.push_back(info);
    }

    pMesh->Release();

    KG3DSceneEntity Entity(SCENEENTITY_MODEL,m_pModel);
    m_listRenderEntity.push_back(Entity);
    
	//AddSelectableEntity(SCENEENTITY_MODEL,m_pModel);
    //AddSelectedEntity(Entity);

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	selTool.SetSelectable(Entity, TRUE);
	selTool.SetSelected(Entity, TRUE);

    return S_OK;
}

HRESULT KG3DSceneSFXEditor::Init()
{
    HRESULT hr = E_FAIL;
    TCHAR strOutputInfo[MAX_PATH*2];
    strOutputInfo[0] = '\0';
    hr = KG3DSceneEditorBase::Init();
    KG_COM_PROCESS_ERROR(hr);

    //
   /*
         SAFE_DELETE(m_lpEnvironment);
            _ASSERTE(_CrtCheckMemory());
            m_lpEnvironment = new KG3DEnvironment; 
            _ASSERTE(_CrtCheckMemory());
            if (!m_lpEnvironment)
                return E_FAIL;*/
        
    //

	/*///现在Fog变成了KG3DEnvEffFog，不会默认加载，需要用KG3DEnvironment来添加
	///这些参数在添加了Fog之后再用RequestModification来控制
    m_Fog.m_bFogEnable = FALSE;
    m_Fog.m_fFogStartValue = 1000;
    m_Fog.m_fFogEndValue = 8000;
    m_Fog.m_fFogDensity = 0.5F;
    KG3DScene::SetFogColor( 0xFFFFFFFF );*/
    

    //Add default light here
   /*//现在Light由LightSet默认创建
    hr = NewOneLight(&m_lpSunlight);
       KG_COM_PROCESS_ERROR(hr);
       m_lpSunlight->InitDefault();*/
   
    this->SetAmbient(0xff999999);


    //
    // Create D3D Font 
    //

    HDC hDC;
    int nHeight;
    int nPointSize = 12;

    hDC = GetDC( NULL );
    nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

    ReleaseDC( NULL, hDC );
    hr = D3DXCreateFont( g_pd3dDevice, nHeight, 0, FW_BOLD, 0, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), 
        &m_lpD3DFont );

   /* D3DXCreateSphere(
        g_pd3dDevice,
        10.0f, 
        50, 
        50, 
        &m_pBindFlagMesh,
        NULL
   );*/

    IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
    if (pSoundShell)
        pSoundShell->Unlock();


    hr = KG3DD3DXCreateEffectFromFile(
        g_pd3dDevice, 
        "\\data\\public\\shader\\sfx_editor.fx", 
        NULL, 
        NULL, 
        0, 
        NULL, 
        &m_pEffect, 
        NULL);
    KG_COM_PROCESS_ERROR(hr);

    if (SUCCEEDED(g_cModelTable.LoadResourceFromFile("\\data\\public\\SfxEditor\\scanler.mesh", 0, 0, (IKG3DResourceBase**)&m_pScanlModel)))
    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pScanlModel);
        D3DXVECTOR3 vScanl = D3DXVECTOR3(10.f, 10.f, 10.f);
        Entity.SetScaling(&vScanl);
    }
    


    hr = S_OK;
Exit0:
    return hr;

    return S_OK;
}

HRESULT KG3DSceneSFXEditor::LoadMaterial(LPSTR pszMtlFile)
{
    ASSERT(pszMtlFile);
    
    if (m_pModel == NULL )
        return E_FAIL;

    return m_pModel->LoadMaterialFromFile(pszMtlFile, 0);
}

HRESULT KG3DSceneSFXEditor::LoadAnimation(LPSTR pszAniFile)
{
    ASSERT(pszAniFile);

    if (m_pModel == NULL )
        return E_FAIL;

    return m_pModel->LoadAnimationFromFile(pszAniFile, 0);
}

HRESULT KG3DSceneSFXEditor::RemoveSFXEntity(DWORD dwIndex)
{
	//找到第几个Entity
    /*KG3DSceneEntityList::iterator i = m_listSelectableEntity.begin();
    advance(i, dwIndex);
    KG3DSceneEntity &Entity = *i;*/

	struct CustomVisitor : public KG3DSceneEntityVisitor
	{
		DWORD m_dwIndex;
		DWORD m_dwCount;
		KG3DSceneEntity m_entityToRemove;
		virtual HRESULT Accept(KG3DSceneEntity& entity)
		{
			++m_dwCount;
			if (m_dwCount >= m_dwIndex)
			{
				m_entityToRemove = entity;
				return E_FAIL;
			}
			return S_OK;
		}
	};
	CustomVisitor visitor;
	visitor.m_dwCount = 0;
	visitor.m_dwIndex = dwIndex;

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	
	selTool.TraverseSelectable(visitor);

	KG_PROCESS_ERROR(visitor.m_dwCount == dwIndex);
	
    return RemoveSFXEntity(visitor.m_entityToRemove);
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSFXEditor::RemoveSFXEntity(KG3DSceneEntity &Entity)
{
    HRESULT hResult = E_FAIL;

    m_listPostRenderEntity.remove(Entity);
    m_listRenderEntity.remove(Entity);
    
	//m_listSelectableEntity.remove(Entity);
	this->GetSelectionTool().SetSelectable(Entity, FALSE);
    //m_listSelectedEntity.remove(Entity);  
	this->GetSelectionTool().SetSelected(Entity, FALSE);

    switch (Entity.GetType())
    {
    case SCENEENTITY_SFX_LAUNCHER:
        {
            DWORD dwCnt = m_lpSFX->GetParticleLauncherNum();
            for (DWORD i = 0; i < dwCnt; ++i)
            {
                KG3DSFXParticleLauncher *pLauncher = NULL;
                m_lpSFX->GetParticleLauncher(i, &pLauncher);
                if (!pLauncher)
                    return E_FAIL;

                if (Entity.m_pPointer == pLauncher)
                    RemoveLauncher(i);
            }
        }
        break;
    case SCENEENTITY_SFX_BILLBOARD:
        {
            DWORD dwCnt = m_lpSFX->GetBillboardNum();
            for (DWORD i = 0; i < dwCnt; ++i)
            {
                KG3DSFXBillboard *pBillboard = NULL;
                m_lpSFX->GetBillboard(i, &pBillboard);
                if (!pBillboard)
                    return E_FAIL;

                if (Entity.m_pPointer == pBillboard)
                    RemoveBillboard(i);
            }
        }
        break;
    case SCENEENTITY_MODEL:
        {
            DWORD dwCnt = m_lpSFX->GetNumModels();
            for (DWORD i = 0; i < dwCnt; ++i)
            {
                KG3DModel *pModel = m_lpSFX->GetModel(i);
                if (!pModel)
                    return E_FAIL;

                if (Entity.m_pPointer == pModel)
                    RemoveModel(i);
            }
           /* if (Entity.m_pPointer == m_pRefrenceModel)
            {
                SAFE_RELEASE(m_pRefrenceModel);
            }*/

            for (DWORD i = 0; i < m_vecRefModels.size(); i++)
            {
                if (Entity.m_pPointer == m_vecRefModels[i])
                {
                    if (Entity.m_pPointer == m_pCurBeBindMdl)
                    {
                        m_nLinkingFlag = BIND_STATE_FREE;
                        m_pCurBeBindMdl = NULL;
                    }
                    else if (Entity.m_pPointer == m_pCurBindModel)
                        m_pCurBindModel = NULL;

                    SAFE_RELEASE(m_vecRefModels[i]); 
                    m_vecRefModels.erase(m_vecRefModels.begin() + i);             
                }
            }           
            if (Entity.m_pPointer == m_adereSource.pModel)
                m_adereSource.pModel = NULL;
            if (Entity.m_pPointer == m_adereTarget.pModel)
                m_adereTarget.pModel = NULL;
        }
        break;
        //if (Entity.m_pPointer == m_lpSFX->GetModel())
        //    RemoveModel();
        //break;
    case SCENEENTITY_SFX_TRACK:
        {
            DWORD dwCnt = m_lpSFX->GetTrackNum();
            for (DWORD i = 0; i < dwCnt; ++i)
            {
                KG3DSFXTrack *pTrack = NULL;
                m_lpSFX->GetTrack(i, &pTrack);
                if (!pTrack)
                    return E_FAIL;

                if (Entity.m_pPointer == pTrack)
                    RemoveTrack(i);
            }
        }
        break;
    case SCENEENTITY_SFX_METEOR:
        {
            DWORD dwCnt = m_lpSFX->GetMeteorNum();
            ASSERT(dwCnt);
            IEKG3DSFXMeteor *piMeteor = NULL;
            KG3DSFXMeteor *pMeteor = NULL;

            for (DWORD i = 0; i < dwCnt; ++i)
            {    
                piMeteor = NULL;

                m_lpSFX->GetMeteor(i, &piMeteor);
                KGLOG_PROCESS_ERROR(piMeteor);
                
                pMeteor = dynamic_cast<KG3DSFXMeteor *>(piMeteor);
                KGLOG_PROCESS_ERROR(pMeteor);

                if (Entity.m_pPointer == pMeteor)
                    RemoveMeteor(i);
            }
        }
        break;
    default:
       // ASSERT(0);
        return E_FAIL;
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

//HRESULT KG3DSceneSFXEditor::LoadBindSFXModel(LPSTR pszSFX)
//{
//    ASSERT(pszSFX);
//    KG3DModel *pModel = NULL;
//    HRESULT hRet = E_FAIL;
//
//    HRESULT hr = g_cModelTable.Get1NewResource((KG3DModel**)&pModel, MESHTYPE_SFX, 0);
//    KG_COM_PROCESS_ERROR(hr);
//
//    pModel->m_scName = pszSFX;
//    hr = pModel->LoadFromFile(pszSFX, 0, 0);
//    KG_COM_PROCESS_ERROR(hr);
//
//    KG3DSFX* psfx = (KG3DSFX*)pModel;
//    psfx->PlayAnimation(SFX_AP_LOOP);
//
//    hRet = S_OK;
//
//Exit0:
//    if (FAILED(hRet))
//    {
//        SAFE_RELEASE(pModel);
//    }
//    else
//    {
//        KG3DSceneEntity Entity1(SCENEENTITY_MODEL, m_pSFXModel);
//        m_listRenderEntity.remove(Entity1);
//        m_listSelectableEntity.remove(Entity1);
//        m_listSelectedEntity.remove(Entity1);
//
//        KG3DSceneEntity Entity2(SCENEENTITY_MODEL, pModel);
//        m_listRenderEntity.push_back(Entity2);
//        m_listSelectableEntity.push_back(Entity2);
//        m_listSelectedEntity.push_back(Entity2);
//
//        SAFE_RELEASE(m_pSFXModel);
//        m_pSFXModel = pModel;
//    }
//
//    return hRet;  
//}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneSFXEditor::SetCurBindModel(IEKG3DModel *piModel)
{
    HRESULT hResult = E_FAIL;
    KG3DModel *pModel = NULL;

    KGLOG_PROCESS_ERROR(piModel);

    pModel = dynamic_cast<KG3DModel *>(piModel);
    KGLOG_PROCESS_ERROR(pModel);

    m_pCurBindModel = pModel;

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneSFXEditor::GetD3DFillModel(DWORD *pdwD3DFillModel)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwD3DFillModel);

    *pdwD3DFillModel = m_dwD3DFillModel;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSceneSFXEditor::SetD3DFillModel(DWORD dwD3DFillModel)
{
    m_dwD3DFillModel = dwD3DFillModel;
    return S_OK;
}
FLOAT KG3DSceneSFXEditor::GetRotationCoordAngelX()
{
    return this->GetCoordRotate().GetRotationAngleX();
}
FLOAT KG3DSceneSFXEditor::GetRotationCoordAngelY()
{
    return this->GetCoordRotate().GetRotationAngleY();
}
FLOAT KG3DSceneSFXEditor::GetRotationCoordAngelZ()
{
    return this->GetCoordRotate().GetRotationAngleZ();
}


BOOL KG3DSceneSFXEditor::OpenModel2Bind(LPCTSTR szModelName, LPCTSTR szBindName)
{
    BOOL bResult     = FALSE;
    HRESULT hRetCode = E_FAIL;

    static D3DXVECTOR3 vTrs = D3DXVECTOR3(0.f, 0.f, 0.f);

    KGLOG_PROCESS_ERROR(szModelName);
    KGLOG_PROCESS_ERROR(szBindName);

    SAFE_RELEASE(m_pHandModel);
    hRetCode = g_cModelTable.LoadResourceFromFile(
        szModelName, 0, MLO_MULTI, (IKG3DResourceBase **)&m_pHandModel
        );
    KG_COM_PROCESS_ERROR(hRetCode);
    
    m_pHandModel->SetTranslation(&vTrs);
    m_pHandModel->FrameMove();
    m_pHandModel->Render(0, NULL);

    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pHandModel);
        //m_listSelectedEntity.push_back(Entity);
		this->GetSelectionTool().SetSelected(Entity, TRUE);
    }

    ImportSuedeToSelMdl(szBindName);
    AddHandMdlToRefMdl();

    //m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();

    bResult = TRUE;
Exit0:
    return bResult;
}


HRESULT KG3DSceneSFXEditor::ModifyBindInfo(IKG3DModel* pModel, IKG3DModel* pBind, int nModifyIndex)
{
    HRESULT hResult  = E_FAIL;

    KG3DSFXSuede* pSuede = dynamic_cast<KG3DSFXSuede*>(pBind);
    KG3DModel* pRefModel = dynamic_cast<KG3DModel*>(pModel);
	static D3DXVECTOR3 vTrs = D3DXVECTOR3(0.f, 0.f, 0.f);
    size_t uBindInfoCnt = 0;

    KGLOG_PROCESS_ERROR(pRefModel);
    KGLOG_PROCESS_ERROR(pSuede);

    uBindInfoCnt = pSuede->GetModelCount();

    KGLOG_PROCESS_ERROR(nModifyIndex >= 0);
    KGLOG_PROCESS_ERROR(nModifyIndex < (int)uBindInfoCnt);

    SAFE_RELEASE(m_pHandModel);
    SAFE_RELEASE(m_pModifySuede);

    for (size_t i = 0; i < m_vecRefModels.size(); i++)
        SAFE_RELEASE(m_vecRefModels[i]);
    m_vecRefModels.clear();

    m_listRenderEntity.clear();
    
	//m_listSelectableEntity.clear();
	this->GetSelectionTool().ClearSelectable();

    m_listPostRenderEntity.clear();
    //m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();
    
    m_pModifySuede = pSuede;
    m_pModifySuede->AddRef();


    m_pHandModel = pRefModel;
    KGLOG_PROCESS_ERROR(m_pHandModel);

    m_pHandModel->AddRef();

    m_pHandModel->SetTranslation(&vTrs);
    m_pHandModel->FrameMove();
    //m_pHandModel->Render(0, NULL);

    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pHandModel);
		this->GetSelectionTool().SetSelected(Entity, TRUE);
        //m_listSelectedEntity.push_back(Entity);
    }

    pSuede->ApplyBind(m_pHandModel, nModifyIndex);

	m_pCurBindModel = m_pHandModel;
    AddHandMdlToRefMdl();

    m_dwSceneMode = SFX_SCENE_MODE_BIND;
    m_bIsModify = TRUE;


    m_pCurBeBindMdl = dynamic_cast<KG3DModel*>(m_pModifySuede->GetModel(nModifyIndex));
    m_pCurBeBindMdl->AddRef();

    m_nModifyIndex  = nModifyIndex;

    m_vecRefModels.push_back(m_pCurBeBindMdl);
    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pCurBeBindMdl);

        m_listRenderEntity.remove(Entity);
        
		//m_listSelectableEntity.remove(Entity);
		this->GetSelectionTool().SetSelectable(Entity, FALSE);

        m_listPostRenderEntity.remove(Entity);

        if (m_pCurBeBindMdl->m_nIsSockWave)
        {
            m_listPostRenderEntity.push_back(Entity);
            if (m_pCurBeBindMdl->m_dwShockMode == SHOCK_WAVE_ADD)
                m_listRenderEntity.push_back(Entity);
        }
        else
            m_listRenderEntity.push_back(Entity);
        
		//m_listSelectableEntity.push_back(Entity);
		this->GetSelectionTool().SetSelectable(Entity, TRUE);
    }


    hResult = S_OK;
Exit0:
    return hResult;
}


bool KG3DSceneSFXEditor::ImportSfx(
    SfxImportInfo& info, DWORD dwImpFrameNum, DWORD dwImpInterval, DWORD dwPrecept, BYTE* pBuffer
    )
{
    ASSERT(pBuffer);
    DWORD texNum = m_lpSFX->GetTextureNum();
    DWORD kndNum = m_lpSFX->GetParticleLifeTimeLineNum();
    DWORD focNum = m_lpSFX->GetForceFieldNum();
    //DWORD luaNum = m_lpSFX->GetParticleLauncherNum();
    DWORD dwCurFrameNum = m_lpSFX->GetTotalFrameNum();
    //DWORD dwCurInterval = m_lpSFX->GetFrameInterval();
    //if (dwCurFrameNum * dwCurInterval >= dwImpFrameNum * dwImpInterval)
    //    dwPrecept = SFX_IMPORT_CUT_IMPSFX;

    if (dwPrecept == SFX_IMPORT_SCANL_CURSFX)
        m_lpSFX->ScanlAllTimeLine(
            static_cast<float>(dwImpFrameNum) / 
            static_cast<float>(dwCurFrameNum)
        );

    for (size_t i = 0; i < info.size(); ++i)
    {
        SfxImportElement e = info[i];

        switch (e.dwType)
        {
        case SFX_FEID_PARTICLE_KIND :
            {
                SFX_PARTICLE_KIND_BLOCK *pBlock = 
                    (SFX_PARTICLE_KIND_BLOCK*)&pBuffer[info[i].dwOffset];
                pBlock->dwTextureIndex += texNum;
            }
            break;
        case SFX_FEID_LAUNCHER :
            {
                SFX_PARTICLE_LAUNCHER_BLOCK *pBlock = 
                    (SFX_PARTICLE_LAUNCHER_BLOCK*)&pBuffer[info[i].dwOffset];
                pBlock->dwParticleKindIndex += kndNum;
                for (int v = 0; v < 32; ++v)
                {
                    if (HasForceBit(pBlock->dwEffForceBit, v))
                    {
                        DelForceBit(pBlock->dwEffForceBit, v);
                        SetForceBit(pBlock->dwEffForceBit, v + focNum);
                    }
                }
            }
            break;
        default :
            break;
        }

        if (e.dwType != SFX_FEID_GENERAL_INFORMATION)
            if (FAILED((m_lpSFX->*KG3DSFX::ms_pfnProcessBlock[e.dwType])(pBuffer, e.dwOffset)))
                continue;


        switch (e.dwType) 
        {
        case SFX_FEID_PARTICLE_KIND :
            {
                KG3DSFXParticleLifeTimeLine* pParticleKind = NULL;
                m_lpSFX->GetParticleLifeTimeLine(
                    m_lpSFX->GetParticleLifeTimeLineNum() - 1, (IEKG3DSFXParticleLifeTimeLine**)&pParticleKind);
                ASSERT(pParticleKind);

                if (dwPrecept == SFX_IMPORT_SCANL_IMPSFX)
                {
                    pParticleKind->ScanlTimeLine(
                        static_cast<float>(dwCurFrameNum) / 
                        static_cast<float>(dwImpFrameNum)
                        );
                }
            }
            break;
        case SFX_FEID_FORCE_FIELD :
            {
                KG3DSFXParticleForceField* pForce = NULL;
                m_lpSFX->GetForceField(m_lpSFX->GetForceFieldNum() - 1, (IEKG3DSFXParticleForceField**)&pForce);
                ASSERT(pForce);

                if (dwPrecept == SFX_IMPORT_SCANL_IMPSFX) 
                {
                    pForce->ScanlTimeLine(
                        static_cast<float>(dwCurFrameNum) / 
                        static_cast<float>(dwImpFrameNum)
                        );
                }
            }
            break;
        case SFX_FEID_BILLBOARD :
            {
                KG3DSFXBillboard *pBillboard = NULL;
                m_lpSFX->GetBillboard(m_lpSFX->GetBillboardNum() - 1, &pBillboard);
                ASSERT(pBillboard);

                SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
                m_vecBillboardEntityData.push_back(pData);
                KG3DSceneEntity Entity(SCENEENTITY_SFX_BILLBOARD, pBillboard, pData);
                
				//m_listSelectableEntity.push_back(Entity);
				this->GetSelectionTool().SetSelectable(Entity, TRUE);

                if (dwPrecept == SFX_IMPORT_SCANL_IMPSFX)
                {
                    pBillboard->ScanlTimeLine(
                        static_cast<float>(dwCurFrameNum) / 
                        static_cast<float>(dwImpFrameNum)
                        );
                }
            }
            break;
        case SFX_FEID_LAUNCHER :
            {
                KG3DSFXParticleLauncher *pLauncher = NULL;
                m_lpSFX->GetParticleLauncher(m_lpSFX->GetParticleLauncherNum() - 1, &pLauncher);
                ASSERT(pLauncher);

                SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
                m_vecLauncherEntityData.push_back(pData);

                KG3DSceneEntity Entity(SCENEENTITY_SFX_LAUNCHER, pLauncher, pData);
                m_listRenderEntity.push_back(Entity);
                
				//m_listSelectableEntity.push_back(Entity);
				this->GetSelectionTool().SetSelectable(Entity, TRUE);

                if (dwPrecept == SFX_IMPORT_SCANL_IMPSFX)
                {
                    pLauncher->ScanlTimeLine(
                        static_cast<float>(dwCurFrameNum) / 
                        static_cast<float>(dwImpFrameNum)
                        );
                }
            }
            break;
        case SFX_FEID_TRACK :
            {
                KG3DSFXTrack *pTrack = NULL;
                m_lpSFX->GetTrack(m_lpSFX->GetTrackNum() - 1, &pTrack);
                ASSERT(pTrack);

                SFX_ENTITY_DATA *pData = _KG_CreateSFXEntityData();
                m_vecTrackEntityData.push_back(pData);
                KG3DSceneEntity Entity(SCENEENTITY_SFX_TRACK, pTrack, pData);
                m_listRenderEntity.push_back(Entity);
                
				//m_listSelectableEntity.push_back(Entity);
				this->GetSelectionTool().SetSelectable(Entity, TRUE);

                SFX_TRACK_BIND_INFORMATION info;
                m_vecTrackBindInformation.push_back(info);
            }
            break;
        case SFX_FEID_MODEL :
            {
                KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_lpSFX->GetModel(m_lpSFX->GetNumModels() - 1));
                
				//m_listSelectableEntity.push_back(Entity);
				this->GetSelectionTool().SetSelectable(Entity, TRUE);

                m_lpSFX->GetModel(m_lpSFX->GetNumModels() - 1)->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
                IKG3DAnimationController* pController = m_lpSFX->GetModel(m_lpSFX->GetNumModels() - 1)->GetAnimationController(ANICTL_PRIMARY);
                pController->PauseAnimation(TRUE);

                if (dwPrecept == SFX_IMPORT_SCANL_IMPSFX)
                {
                    KG3DSFX::MDL_COLOR_LINE_T* pColorLine = 
                        m_lpSFX->m_vecSubsetColor[m_lpSFX->m_vecSubsetColor.size() - 1];
                    ASSERT(pColorLine);
                    for (size_t i = 0; i < pColorLine->size(); ++i)
                    {
                        (*pColorLine)[i]->Scanl(
                            static_cast<float>(dwCurFrameNum) / 
                            static_cast<float>(dwImpFrameNum)
                            );
                    }
                }
            }
            break;
        case SFX_FEID_METEOR :
            {
                KG3DSFXMeteor *pMeteor = NULL;
                m_lpSFX->GetMeteor(m_lpSFX->GetMeteorNum() - 1, (IEKG3DSFXMeteor**)&pMeteor);
                ASSERT(pMeteor);

                KG3DSceneEntity Entity(SCENEENTITY_SFX_METEOR, pMeteor);
                m_listRenderEntity.push_back(Entity);
                
				//m_listSelectableEntity.push_back(Entity);           
				this->GetSelectionTool().SetSelectable(Entity, TRUE);
            }
            break;
        default :
            break;
         }
    }

    return true;
}

void KG3DSceneSFXEditor::OnEntityTransformation(IEKG3DSceneEntity* pEntity, int nFrame)
{
    KG3DModel* pModel = NULL;

    pEntity->GetPointer((void**)&pModel);

    if (pModel == m_pScanlModel)
        InsertExpScanlKeyFrame(nFrame);
}

void KG3DSceneSFXEditor::GetScanlModelScanl(D3DXVECTOR3* pvScanl)
{
    if (!pvScanl || !m_pScanlModel)
        return;
    m_pScanlModel->GetScaling(pvScanl);
}

void KG3DSceneSFXEditor::InsertExpScanlKeyFrame(int nFrame)
{
    if (!m_pScanlModel || !m_lpSFX || !m_lpSFX->m_pExpEffect)
        return;

    D3DXVECTOR3 vScanl;
    m_pScanlModel->GetScaling(&vScanl);
    vScanl /= 10.f;
    m_lpSFX->m_pExpEffect->m_tlModelScanl.InsertKeyFrame(nFrame, vScanl);
}

void KG3DSceneSFXEditor::ShowExpScanlModel(BOOL bShow)
{
    if (!m_pScanlModel || !m_lpSFX)
        return;

    KG3DSceneEntity Entity(SCENEENTITY_MODEL, m_pScanlModel);

    if (bShow)
    {
        AddRenderEntity(Entity);
        
        KG3DSfxExp* pExp = (KG3DSfxExp*)m_lpSFX->GetExpEffect();

        if (pExp)
        {
            D3DXVECTOR3 vScanl = D3DXVECTOR3(10.f, 10.f, 10.f);
            D3DXVECTOR3 vSave;
            Entity.GetScaling(&vSave);
            Entity.SetScaling(&vScanl);
            pExp->OnSfxBind(m_pScanlModel);
            Entity.SetScaling(&vSave);
        }
    }
    else
    {
        RemoveRenderEntity(Entity);

        KG3DSfxExp* pExp = (KG3DSfxExp*)m_lpSFX->GetExpEffect();

        if (pExp)
        {
            pExp->OnSfxUnbind();
        }
    }
}

void KG3DSceneSFXEditor::UpdateSfxExp()
{
    if (m_lpSFX && m_lpSFX->m_pExpEffect)
        m_lpSFX->m_pExpEffect->FrameMove(m_lpSFX->GetCurrentFrame());
}

HRESULT KG3DSceneSFXEditor::AdhereToModelEx(
					  IEKG3DModel*  pSourceModel,
					  LPCTSTR       strSurEFName,
					  DWORD       dwSurType,
					  IEKG3DModel*  pTargetModel,
					  LPCTSTR      strTarEFName,
					  DWORD       dwTarType
					  )
{
	return AdhereToModel(static_cast<KG3DModel*>(pSourceModel),
		strSurEFName,
		dwSurType,
		static_cast<KG3DModel*>(pTargetModel),
		strTarEFName,
		dwTarType
		);
}

BOOL KG3DSceneSFXEditor::HaveModel(IEKG3DModel* pModel)
{
	for (DWORD i = 0; i < m_vecRefModels.size(); i++)
	{
		if (pModel == m_vecRefModels[i])
			return TRUE;
	}        
	return FALSE;
}


HRESULT KG3DSceneSFXEditor::LoadBGTexture(const char filename[])
{
	SAFE_RELEASE(m_pBGTexture);
	if(FAILED(g_cTextureTable.LoadResourceFromFile(
		filename, 0, TEXOP_DISABLEDTX, (IKG3DResourceBase**)&m_pBGTexture
		)))
		return E_FAIL;

	return S_OK;
}

BOOL KG3DSceneSFXEditor::IsShowBGTexture()
{
	return m_bShowBGTexture;
}

void KG3DSceneSFXEditor::EnableShowBGTexture(BOOL val)
{
	m_bShowBGTexture = val;
}

HRESULT KG3DSceneSFXEditor::GetSelectionToolInterface( IEKG3DSceneSelectionTool** ppTool )
{
	return KG3DScene::GetSelectionToolInterface(ppTool);
}

HRESULT KG3DSceneSFXEditor::AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable)
{
	KG3DSceneEntity Entity(SCENEENTITY_MODEL,pEntity);
	m_listPostRenderEntity.push_back(Entity);
	return S_OK;
}