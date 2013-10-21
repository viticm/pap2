#include "stdafx.h"
#include "./kmodel.h"
#include "./krlresource.h"
#include "./kanimationmodel.h"
#include "../../SO3Represent.h"
#include "../../base/file/kfilepath.h"
#include "../../case/actionobject/krlsceneobject.h"
#include "../../case/actionobject/krlscene.h"

KAnimationModel::KAnimationModel()
{
    memset(m_ModelSlots, 0, sizeof(m_ModelSlots));
}

KAnimationModel::~KAnimationModel()
{
    for (size_t nIndex = 0; nIndex < m_ModelData.RLModels.size(); ++nIndex)
    {
        ASSERT(!HasLoadedModel(nIndex));
        KGLOG_CHECK_ERROR(!HasLoadedModel(nIndex));
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLSockets.size(); ++nIndex)
    {
        ASSERT(!HasLoadedSocket(nIndex));
        KGLOG_CHECK_ERROR(!HasLoadedSocket(nIndex));
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        ASSERT(!HasLoadedMDLSFX(nIndex));
        KGLOG_CHECK_ERROR(!HasLoadedMDLSFX(nIndex));
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLEquipSFXs.size(); ++nIndex)
    {
        ASSERT(!HasLoadedEquipSFX(nIndex));
        KGLOG_CHECK_ERROR(!HasLoadedEquipSFX(nIndex));
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLSocketSFXs.size(); ++nIndex)
    {
        ASSERT(!HasLoadedSocketSFX(nIndex));
        KGLOG_CHECK_ERROR(!HasLoadedSocketSFX(nIndex));
    }

    ASSERT(!HasLoadedMDL());
    KGLOG_CHECK_ERROR(!HasLoadedMDL());
}

HRESULT KAnimationModel::TransferOwnershipFrom(KAnimationModel& rhs)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(m_pRLScene == rhs.m_pRLScene);
    ASSERT(m_pRLSceneObject);

    // unregister
    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLSocketSFXs.size(); ++nIndex)
    {
        if (rhs.m_ModelData.RLSocketSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::UnregisterAnimationEventHandler(
            rhs.m_pRLScene, 
            rhs.m_ModelData.RLSocketSFXs[nIndex].p3DModel, 
            rhs.m_pRLSceneObject, 
            rhs.m_ModelData.RLSocketSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLEquipSFXs.size(); ++nIndex)
    {
        if (rhs.m_ModelData.RLEquipSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::UnregisterAnimationEventHandler(
            rhs.m_pRLScene, 
            rhs.m_ModelData.RLEquipSFXs[nIndex].p3DModel, 
            rhs.m_pRLSceneObject, 
            rhs.m_ModelData.RLEquipSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        if (rhs.m_ModelData.RLMDLSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::UnregisterAnimationEventHandler(
            rhs.m_pRLScene, 
            rhs.m_ModelData.RLMDLSFXs[nIndex].p3DModel, 
            rhs.m_pRLSceneObject, 
            rhs.m_ModelData.RLMDLSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (rhs.m_ModelData.RLMDL.p3DModel)
    {
        hr = KModel::UnregisterAnimationEventHandler(
            rhs.m_pRLScene, 
            rhs.m_ModelData.RLMDL.p3DModel, 
            rhs.m_pRLSceneObject, 
            rhs.m_ModelData.RLMDL.bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    // move
    memcpy(m_ModelSlots, rhs.m_ModelSlots, sizeof(rhs.m_ModelSlots));
    memset(rhs.m_ModelSlots, 0, sizeof(rhs.m_ModelSlots));

    m_ModelData = rhs.m_ModelData;

    rhs.m_ModelData.RLMDL.p3DModel = NULL;
    rhs.m_ModelData.RLModels.resize(0);
    rhs.m_ModelData.RLSockets.resize(0);
    rhs.m_ModelData.RLMDLSFXs.resize(0);
    rhs.m_ModelData.RLEquipSFXs.resize(0);
    rhs.m_ModelData.RLSocketSFXs.resize(0);

    // register
    if (m_ModelData.RLMDL.p3DModel)
    {
        hr = KModel::RegisterAnimationEventHandler(
            m_pRLScene, 
            m_ModelData.RLMDL.p3DModel, 
            m_pRLSceneObject, 
            m_ModelData.RLMDL.bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLMDLSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::RegisterAnimationEventHandler(
            m_pRLScene, 
            m_ModelData.RLMDLSFXs[nIndex].p3DModel, 
            m_pRLSceneObject, 
            m_ModelData.RLMDLSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLEquipSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLEquipSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::RegisterAnimationEventHandler(
            m_pRLScene, 
            m_ModelData.RLEquipSFXs[nIndex].p3DModel, 
            m_pRLSceneObject, 
            m_ModelData.RLEquipSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < rhs.m_ModelData.RLSocketSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLSocketSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::UnregisterAnimationEventHandler(
            m_pRLScene, 
            m_ModelData.RLSocketSFXs[nIndex].p3DModel, 
            m_pRLSceneObject, 
            m_ModelData.RLSocketSFXs[nIndex].bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetMDLSelectable(BOOL bSelectable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	KSceneObject* pSceneObject = NULL;
	DWORD dwObject = 0;
	DWORD dwObjectType = 0;
    IKG3DModel* p3DModel = NULL;
    BOOL bPreviousSelectable = FALSE;

    bPreviousSelectable = m_ModelData.RLMDL.bSelectable;
    m_ModelData.RLMDL.bSelectable = bSelectable;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);
    KGLOG_PROCESS_ERROR(m_pRLSceneObject);

    KG_PROCESS_SUCCESS(bPreviousSelectable == bSelectable);

    if (bPreviousSelectable)
    {
        hr = m_pRLScene->m_ModelMgr.UnregisterModel(p3DModel);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    hr = KModel::RemoveRenderEntity(m_pRLScene->m_p3DScene, p3DModel, bPreviousSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::AppendRenderEntity(m_pRLScene->m_p3DScene, p3DModel, bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (bSelectable)
    {
        pSceneObject = m_pRLSceneObject->GetGameWorldSceneObject();
        KGLOG_PROCESS_ERROR(pSceneObject);

        dwObject = m_pRLSceneObject->GetObjectID();
        dwObjectType = m_pRLSceneObject->GetObjectType();

        hr = m_pRLScene->m_ModelMgr.RegisterModel(p3DModel, pSceneObject, dwObjectType, dwObject);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetMDLVisible(BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;
    BOOL bPreviousVisible = FALSE;

    bPreviousVisible = m_ModelData.RLMDL.bVisible;
    m_ModelData.RLMDL.bVisible = bVisible;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);

    KG_PROCESS_SUCCESS(bPreviousVisible == bVisible);

    if (bPreviousVisible)
    {
        hr = KModel::RemoveRenderEntity(m_pRLScene->m_p3DScene, m_ModelData.RLMDL.p3DModel, m_ModelData.RLMDL.bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (bVisible)
    {
        hr = KModel::SetPosition(m_ModelData.RLMDL.p3DModel, m_ModelData.RLMDL.vPosition);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::AppendRenderEntity(m_pRLScene->m_p3DScene, m_ModelData.RLMDL.p3DModel, m_ModelData.RLMDL.bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetMDLSFXInfo(size_t nIndex, BOOL bBind)
{
    ASSERT(nIndex < m_ModelData.RLMDLSFXs.size());
    m_ModelData.RLMDLSFXs[nIndex].bBind = bBind;
    return S_OK;
}

HRESULT KAnimationModel::UnloadAllModel()
{
    HRESULT hr = E_FAIL;

    for (int nIndex = static_cast<int>(m_ModelData.RLModels.size() - 1); nIndex >= 0; --nIndex)
    {
        if (HasLoadedModel(static_cast<size_t>(nIndex)))
        {
            hr = UnloadModel(static_cast<size_t>(nIndex));
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    return S_OK;
}

HRESULT KAnimationModel::UnloadAllSocket()
{
    HRESULT hr = E_FAIL;

    for (int nIndex = static_cast<int>(m_ModelData.RLSockets.size() - 1); nIndex >= 0; --nIndex)
    {
        if (HasLoadedSocket(static_cast<size_t>(nIndex)))
        {
            hr = UnloadSocket(static_cast<size_t>(nIndex));
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    return S_OK;
}

HRESULT KAnimationModel::UnloadAllMDLSFX()
{
    HRESULT hr = E_FAIL;

    for (int nIndex = static_cast<int>(m_ModelData.RLMDLSFXs.size() - 1); nIndex >= 0; --nIndex)
    {
        if (HasLoadedMDLSFX(static_cast<size_t>(nIndex)))
        {
            hr = UnloadMDLSFX(static_cast<size_t>(nIndex));
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    return S_OK;
}

HRESULT KAnimationModel::UnloadAllEquipSFX()
{
    HRESULT hr = E_FAIL;

    for (int nIndex = static_cast<int>(m_ModelData.RLEquipSFXs.size() - 1); nIndex >= 0; --nIndex)
    {
        if (HasLoadedEquipSFX(static_cast<size_t>(nIndex)))
        {
            hr = UnloadEquipSFX(static_cast<size_t>(nIndex));
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    return S_OK;
}

HRESULT KAnimationModel::UnloadAllSocketSFX()
{
    HRESULT hr = E_FAIL;

    for (int nIndex = static_cast<int>(m_ModelData.RLSocketSFXs.size() - 1); nIndex >= 0; --nIndex)
    {
        if (HasLoadedSocketSFX(static_cast<size_t>(nIndex)))
        {
            hr = UnloadSocketSFX(static_cast<size_t>(nIndex));
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    return S_OK;
}

HRESULT KAnimationModel::SetMDLSFXPosition(size_t nIndex, D3DXVECTOR3 vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    KG_ASSERT_EXIT(nIndex < m_ModelData.RLMDLSFXs.size());

    m_ModelData.RLMDLSFXs[nIndex].vPosition = vPosition;

    p3DModel = m_ModelData.RLMDLSFXs[nIndex].p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetPosition(p3DModel, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetMDLSFXOffsetDirection(size_t nIndex, D3DXVECTOR3 vOffset)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    KG_ASSERT_EXIT(nIndex < m_ModelData.RLMDLSFXs.size());

    p3DModel = m_ModelData.RLMDLSFXs[nIndex].p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetOffsetDirection(p3DModel, vOffset);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

static int Is3DModelReady(IKG3DModel* p3DModel)
{
    if (p3DModel == NULL)
        return false;

    if (!p3DModel->IsLoaded())
        return false;

    return true;
}

static int IsRLModelInfoListReady(KRLModelInfoList const& RLModelInfoList)
{
    for (KRLModelInfoList::const_iterator it = RLModelInfoList.begin(); it != RLModelInfoList.end(); ++it)
    {
        KRLModelInfo const& RLModelInfo = *it;

        if (!Is3DModelReady(RLModelInfo.p3DModel))
        {
            return false;
        }
    }

    return true;
}

int KAnimationModel::IsReady() const
{
    if (!Is3DModelReady(m_ModelData.RLMDL.p3DModel))
        return false;

    if (!Is3DModelReady(m_ModelData.RLMDL.p3DModel))
        return false;

    if (!IsRLModelInfoListReady(m_ModelData.RLModels))
        return false;

	if (!IsRLModelInfoListReady(m_ModelData.RLSockets))
        return false;

	if (!IsRLModelInfoListReady(m_ModelData.RLMDLSFXs))
        return false;

    if (!IsRLModelInfoListReady(m_ModelData.RLEquipSFXs))
        return false;

	if (!IsRLModelInfoListReady(m_ModelData.RLSocketSFXs))
        return false;

    return true;
}

int KAnimationModel::RemoveCaption()
{
    int nResult = false;

    KG_PROCESS_SUCCESS(m_ModelData.RLMDL.p3DModel == NULL);

    m_ModelData.RLMDL.p3DModel->RemoveCaption();

Exit1:
    nResult = true;
    return nResult;
}

BOOL KAnimationModel::HasLoadedMDL() const
{
    return m_ModelData.RLMDL.p3DModel != NULL;
}

BOOL KAnimationModel::HasLoadedModel(size_t nIndex) const
{
    ASSERT(nIndex < m_ModelData.RLModels.size());
    return m_ModelData.RLModels[nIndex].p3DModel != NULL;
}

BOOL KAnimationModel::HasLoadedSocket(size_t nIndex) const
{
    ASSERT(nIndex < m_ModelData.RLSockets.size());
    return m_ModelData.RLSockets[nIndex].p3DModel != NULL;
}

BOOL KAnimationModel::HasLoadedMDLSFX(size_t nIndex) const
{
    ASSERT(nIndex < m_ModelData.RLMDLSFXs.size());
    return m_ModelData.RLMDLSFXs[nIndex].p3DModel != NULL;
}

BOOL KAnimationModel::HasLoadedEquipSFX(size_t nIndex) const
{
    ASSERT(nIndex < m_ModelData.RLEquipSFXs.size());
    return m_ModelData.RLEquipSFXs[nIndex].p3DModel != NULL;
}

BOOL KAnimationModel::HasLoadedSocketSFX(size_t nIndex) const
{
    ASSERT(nIndex < m_ModelData.RLSocketSFXs.size());
    return m_ModelData.RLSocketSFXs[nIndex].p3DModel != NULL;
}

BOOL KAnimationModel::IsVisible() const
{
    return m_ModelData.RLMDL.p3DModel && m_ModelData.RLMDL.p3DModel->IsVisible();
}

BOOL KAnimationModel::IsMDLSelectable() const
{
    return m_ModelData.RLMDL.bSelectable;
}

BOOL KAnimationModel::LookupModel(IKG3DModel* p3DModel, size_t& nIndex) const
{
    ASSERT(p3DModel);

    for (nIndex = 0; nIndex < m_ModelData.RLModels.size(); ++nIndex)
    {
        if (m_ModelData.RLModels[nIndex].p3DModel == p3DModel)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL KAnimationModel::LookupSocket(IKG3DModel* p3DModel, size_t& nIndex) const
{
    ASSERT(p3DModel);

    for (nIndex = 0; nIndex < m_ModelData.RLSockets.size(); ++nIndex)
    {
        if (m_ModelData.RLSockets[nIndex].p3DModel == p3DModel)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL KAnimationModel::LookupMDLSFX(IKG3DModel* p3DModel, size_t& nIndex) const
{
    ASSERT(p3DModel);

    for (nIndex = 0; nIndex < m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLMDLSFXs[nIndex].p3DModel == p3DModel)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL KAnimationModel::LookupEquipSFX(IKG3DModel* p3DModel, size_t& nIndex) const
{
    ASSERT(p3DModel);

    for (nIndex = 0; nIndex < m_ModelData.RLEquipSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLEquipSFXs[nIndex].p3DModel == p3DModel)
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL KAnimationModel::LookupSocketSFX(IKG3DModel* p3DModel, size_t& nIndex) const
{
    ASSERT(p3DModel);

    for (nIndex = 0; nIndex < m_ModelData.RLSocketSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLSocketSFXs[nIndex].p3DModel == p3DModel)
        {
            return TRUE;
        }
    }

    return FALSE;
}

HRESULT KAnimationModel::Reset()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = ClearSlots();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadAllSocketSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadAllEquipSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadAllMDLSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadAllSocket();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadAllModel();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UnloadMDL();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::Update()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = UpdateSlots();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::Init(KRLSceneObject* pRLSceneObject, size_t nModelCount, size_t nSocketCount, size_t nMDLSFXCount)
{
    ASSERT(pRLSceneObject);

    m_pRLSceneObject = pRLSceneObject;
    m_pRLScene = NULL;

    m_ModelData.RLModels.resize(nModelCount);
    m_ModelData.RLSockets.resize(nSocketCount);
    m_ModelData.RLMDLSFXs.resize(nMDLSFXCount);
    m_ModelData.RLEquipSFXs.resize(nModelCount * 2);
    m_ModelData.RLSocketSFXs.resize(nSocketCount * 2);

    return S_OK;
}

HRESULT KAnimationModel::AttachScene(KRLScene* pRLScene)
{
    KGLOG_PROCESS_ERROR(pRLScene);

    m_pRLScene = pRLScene;

    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KAnimationModel::DetachScene()
{
    m_pRLScene = NULL;
    return S_OK;
}

KRLScene* KAnimationModel::GetRLScene() const
{
    return m_pRLScene;
}

HRESULT KAnimationModel::LoadMDL(LPCTSTR szFilePath, float fScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bLoadMDL = FALSE;
    IKG3DModel* p3DModel = NULL;

    ASSERT(!HasLoadedMDL());
    KGLOG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    hr = KModel::LoadMDL(m_pRLScene, &p3DModel, m_pRLSceneObject, szFilePath, m_ModelData.RLMDL.bVisible, m_ModelData.RLMDL.bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLoadMDL = TRUE;

    hr = KModel::SetScale(p3DModel, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetDetail(p3DModel, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_ModelData.RLMDL.p3DModel = p3DModel;
    p3DModel = NULL;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("KAnimationModel::LoadMDL(%s, %f, %d, %d)\n", szFilePath, fScale, nColorChannelTable, nColorChannel);

        if (bLoadMDL)
        {
            hr = KModel::UnloadMDL(m_pRLScene, &p3DModel, m_pRLSceneObject, m_ModelData.RLMDL.bVisible, m_ModelData.RLMDL.bSelectable);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadMDL()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_SUCCESS(!HasLoadedMDL());

    hr = UnloadAllMDLSFX();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = KModel::UnloadMDL(m_pRLScene, &m_ModelData.RLMDL.p3DModel, m_pRLSceneObject, m_ModelData.RLMDL.bVisible, m_ModelData.RLMDL.bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
    }
    return hrResult;
}

HRESULT KAnimationModel::LoadModel(size_t nIndex, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bLoadModel = FALSE;
    BOOL bAttachModel = FALSE;
    IKG3DModel* p3DModel = NULL;

    KGLOG_PROCESS_ERROR(HasLoadedMDL());

    ASSERT(nIndex < m_ModelData.RLModels.size());
    ASSERT(!HasLoadedModel(nIndex));

    hr = KModel::LoadModel(m_pRLScene, &p3DModel, NULL, szMesh);
    KG_COM_PROCESS_ERROR(hr);

    bLoadModel = TRUE;

    hr = KModel::AttachModel(m_ModelData.RLMDL.p3DModel, p3DModel);
    KG_COM_PROCESS_ERROR(hr);

    bAttachModel = TRUE;

    hr = KModel::SetScale(p3DModel, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    KG_ASSERT_EXIT(szMaterial);

    if (szMaterial[0] != _T('\0'))
    {
        hr = KModel::LoadMaterial(p3DModel, szMaterial);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = KModel::SetDetail(p3DModel, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_ModelData.RLModels[nIndex].p3DModel = p3DModel;
    p3DModel = NULL;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (bAttachModel)
        {
            hr = KModel::DetachModel(m_ModelData.RLMDL.p3DModel, p3DModel);
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (bLoadModel)
        {
            hr = KModel::UnloadModel(m_pRLScene, &p3DModel, NULL);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadModel(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLModels.size());
    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KGLOG_PROCESS_ERROR(HasLoadedModel(nIndex));

    if (HasLoadedEquipSFX(nIndex * 2))
    {
        hr = UnloadEquipSFX(nIndex * 2);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    if (HasLoadedEquipSFX(nIndex * 2 + 1))
    {
        hr = UnloadEquipSFX(nIndex * 2 + 1);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, m_ModelData.RLModels[nIndex].p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::DetachModel(m_ModelData.RLMDL.p3DModel, m_ModelData.RLModels[nIndex].p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::UnloadModel(m_pRLScene, &m_ModelData.RLModels[nIndex].p3DModel, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::LoadSocket(size_t nIndex, LPCTSTR szSocket, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;
    KRLModelInfo* pRLModelInfo = NULL;
    
    KGLOG_PROCESS_ERROR(HasLoadedMDL());

    ASSERT(nIndex < m_ModelData.RLSockets.size());
    
    pRLModelInfo = &m_ModelData.RLSockets[nIndex];

    if (pRLModelInfo->p3DModel)
    {
        KG_PROCESS_SUCCESS(_tcscmp(pRLModelInfo->szSocket, szSocket) == 0 && _tcscmp(pRLModelInfo->szFilePath, szMesh) == 0);

        hr = UnloadSocket(nIndex);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = KModel::LoadModel(m_pRLScene, &p3DModel, NULL, szMesh);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::Bind(m_pRLScene, m_ModelData.RLMDL.p3DModel, p3DModel, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetScale(p3DModel, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (strcmp("S_Face", szSocket) == 0)
    {
        hr = KModel::SetFaceMotionMode(p3DModel);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    KG_ASSERT_EXIT(szMaterial);

    if (szMaterial[0] != _T('\0'))
    {
        hr = KModel::LoadMaterial(p3DModel, szMaterial);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = KModel::SetDetail(p3DModel, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLModelInfo->p3DModel = p3DModel;
    p3DModel = NULL;

    _tcsncpy(pRLModelInfo->szSocket, szSocket, _countof(pRLModelInfo->szSocket));
    pRLModelInfo->szSocket[_countof(pRLModelInfo->szSocket) - 1] = _T('\0');

    _tcsncpy(pRLModelInfo->szFilePath, szMesh, _countof(pRLModelInfo->szFilePath));
    pRLModelInfo->szFilePath[_countof(pRLModelInfo->szFilePath) - 1] = _T('\0');

Exit1:
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (p3DModel)
        {
            hr = KModel::UnloadModel(m_pRLScene, &p3DModel, NULL);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        KFilePath::TabError("LoadSocket(%d, %s, %s, %s, %f, %d, %d)\n", 
            nIndex, szSocket, szMesh, szMaterial, fScale, nColorChannelTable, nColorChannel);
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadSocket(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLModelInfo* pRLModelInfo = NULL;

    ASSERT(nIndex < m_ModelData.RLSockets.size());
    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KGLOG_PROCESS_ERROR(HasLoadedSocket(nIndex));

    if (HasLoadedSocketSFX(nIndex * 2))
    {
        hr = UnloadSocketSFX(nIndex * 2);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (HasLoadedSocketSFX(nIndex * 2 + 1))
    {
        hr = UnloadSocketSFX(nIndex * 2 + 1);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    pRLModelInfo = &m_ModelData.RLSockets[nIndex];

    hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, pRLModelInfo->p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::UnloadModel(m_pRLScene, &pRLModelInfo->p3DModel, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLModelInfo->szSocket[0] = _T('\0');
    pRLModelInfo->szFilePath[0] = _T('\0');

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::LoadMDLSFX(size_t nIndex, KRLSFXParam const& SFXParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    DWORD dwRenderSFXPlayType = 0;
    KRLMonoAnimationParam MonoAnimationParam;
    IKG3DModel* p3DModel = NULL;

    ASSERT(nIndex < m_ModelData.RLMDLSFXs.size());

    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KG_PROCESS_ERROR(SFXParam.szFilePath[0] != _T('\0'));

    if (HasLoadedMDLSFX(nIndex))
    {
        hr = UnloadMDLSFX(nIndex);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_ModelData.RLMDLSFXs[nIndex].bSelectable)
    {
        hr = KModel::LoadModel(m_pRLScene, &p3DModel, m_pRLSceneObject, SFXParam.szFilePath);
        KG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = KModel::LoadMDL(m_pRLScene, &p3DModel, m_pRLSceneObject, SFXParam.szFilePath, FALSE, FALSE);
        KG_COM_PROCESS_ERROR(hr);
    }

    hr = KModel::SetScale(p3DModel, SFXParam.fScale);
    KGLOG_COM_CHECK_ERROR(hr);

    if (m_ModelData.RLMDLSFXs[nIndex].bBind)
    {
        hr = KModel::Bind(m_pRLScene, m_ModelData.RLMDL.p3DModel, p3DModel, SFXParam.szBoneName);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = KModel::SetPosition(p3DModel, m_ModelData.RLMDLSFXs[nIndex].vPosition);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = KModel::AppendRenderEntity(m_pRLScene->m_p3DScene, p3DModel, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    MonoAnimationParam.dwPlayType = SFXParam.dwPlayType;
    MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
    MonoAnimationParam.fSpeed = SFXParam.fSpeed;
    MonoAnimationParam.nOffsetTime = 0;
    MonoAnimationParam.dwTweenTime = 0;
    MonoAnimationParam.pUserdata = NULL;
    MonoAnimationParam.szFilePath = _T("");
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    hr = KModel::PlayMonoAnimation(p3DModel, MonoAnimationParam);
    KG_COM_CHECK_ERROR(hr);

    m_ModelData.RLMDLSFXs[nIndex].p3DModel = p3DModel;
    p3DModel = NULL;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("LoadMDLSFX(%f, %u, %f, %s, %s)\n", SFXParam.fSpeed, SFXParam.dwPlayType, SFXParam.fScale, SFXParam.szFilePath, SFXParam.szBoneName);
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadMDLSFX(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLModelInfo* pRLModelInfo = NULL;
    IKG3DModel* p3DModel = NULL;

    ASSERT(nIndex < m_ModelData.RLMDLSFXs.size());

    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KGLOG_PROCESS_ERROR(HasLoadedMDLSFX(nIndex));

    p3DModel = m_ModelData.RLMDLSFXs[nIndex].p3DModel;
    m_ModelData.RLMDLSFXs[nIndex].p3DModel = NULL;

    pRLModelInfo = &m_ModelData.RLMDLSFXs[nIndex];

    if (pRLModelInfo->bBind)
    {
        hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, p3DModel);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = KModel::RemoveRenderEntity(m_pRLScene->m_p3DScene, p3DModel, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (pRLModelInfo->bSelectable)
    {
        hr = KModel::UnloadModel(m_pRLScene, &p3DModel, m_pRLSceneObject);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = KModel::UnloadMDL(m_pRLScene, &p3DModel, m_pRLSceneObject, FALSE, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::LoadEquipSFX(size_t nIndex, LPCTSTR szFilePath, float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLMonoAnimationParam MonoAnimationParam;
    size_t nEquipID = nIndex / 2;

    ASSERT(nIndex < m_ModelData.RLEquipSFXs.size());

    KGLOG_PROCESS_ERROR(HasLoadedModel(nEquipID));
    KGLOG_PROCESS_ERROR(!HasLoadedEquipSFX(nIndex));
    KG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    hr = KModel::LoadMDL(m_pRLScene, &m_ModelData.RLEquipSFXs[nIndex].p3DModel, m_pRLSceneObject, szFilePath, TRUE, FALSE);
    KG_COM_PROCESS_ERROR(hr);

    hr = KModel::Bind(m_pRLScene, m_ModelData.RLModels[nEquipID].p3DModel, m_ModelData.RLEquipSFXs[nIndex].p3DModel, _T(""));
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetScale(m_ModelData.RLEquipSFXs[nIndex].p3DModel, fScale);
    KGLOG_COM_CHECK_ERROR(hr);

    MonoAnimationParam.dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
    MonoAnimationParam.fSpeed = 1.0f;
    MonoAnimationParam.nOffsetTime = 0;
    MonoAnimationParam.dwTweenTime = 0;
    MonoAnimationParam.pUserdata = NULL;
    MonoAnimationParam.szFilePath = _T("");
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    hr = KModel::PlayMonoAnimation(m_ModelData.RLEquipSFXs[nIndex].p3DModel, MonoAnimationParam);
    KGLOG_COM_CHECK_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("LoadEquipSFX(%u, %s, %f)\n", nIndex, szFilePath, fScale);
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadEquipSFX(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    size_t nEquipID = nIndex / 2;

    ASSERT(nIndex < m_ModelData.RLEquipSFXs.size());
    KGLOG_PROCESS_ERROR(HasLoadedModel(nEquipID));
    KGLOG_PROCESS_ERROR(HasLoadedEquipSFX(nIndex));

    hr = KModel::Unbind(m_pRLScene, m_ModelData.RLModels[nEquipID].p3DModel, m_ModelData.RLEquipSFXs[nIndex].p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::UnloadMDL(m_pRLScene, &m_ModelData.RLEquipSFXs[nIndex].p3DModel, m_pRLSceneObject, TRUE, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::LoadSocketSFX(size_t nIndex, LPCTSTR szFilePath, float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLMonoAnimationParam MonoAnimationParam;
    size_t nSocketID = nIndex / 2;

    ASSERT(nIndex < m_ModelData.RLSocketSFXs.size());
    KGLOG_PROCESS_ERROR(HasLoadedSocket(nSocketID));
    KGLOG_PROCESS_ERROR(!HasLoadedSocketSFX(nIndex));
    KG_PROCESS_ERROR(szFilePath[0] != _T('\0'));

    hr = KModel::LoadMDL(m_pRLScene, &m_ModelData.RLSocketSFXs[nIndex].p3DModel, m_pRLSceneObject, szFilePath, TRUE, FALSE);
    KG_COM_PROCESS_ERROR(hr);

    hr = KModel::Bind(m_pRLScene, m_ModelData.RLSockets[nSocketID].p3DModel, m_ModelData.RLSocketSFXs[nIndex].p3DModel, _T(""));
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetScale(m_ModelData.RLSocketSFXs[nIndex].p3DModel, fScale);
    KGLOG_COM_CHECK_ERROR(hr);

    MonoAnimationParam.dwPlayType = ENU_ANIMATIONPLAY_CIRCLE;
    MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
    MonoAnimationParam.fSpeed = 1.0f;
    MonoAnimationParam.nOffsetTime = 0;
    MonoAnimationParam.dwTweenTime = 0;
    MonoAnimationParam.pUserdata = NULL;
    MonoAnimationParam.szFilePath = _T("");
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    hr = KModel::PlayMonoAnimation(m_ModelData.RLSocketSFXs[nIndex].p3DModel, MonoAnimationParam);
    KGLOG_COM_CHECK_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("LoadSocketSFX(%u, %s, %f)\n", nIndex, szFilePath, fScale);
    }
    return hrResult;
}

HRESULT KAnimationModel::UnloadSocketSFX(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    size_t nSocketID = nIndex / 2;

    ASSERT(nIndex < m_ModelData.RLSocketSFXs.size());
    KGLOG_PROCESS_ERROR(HasLoadedSocket(nSocketID));
    KGLOG_PROCESS_ERROR(HasLoadedSocketSFX(nIndex));

    hr = KModel::Unbind(m_pRLScene, m_ModelData.RLSockets[nSocketID].p3DModel, m_ModelData.RLSocketSFXs[nIndex].p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::UnloadMDL(m_pRLScene, &m_ModelData.RLSocketSFXs[nIndex].p3DModel, m_pRLSceneObject, TRUE, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::BindSocket(size_t nIndex, LPCTSTR szSocket)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLSockets.size());

    nRetCode = HasLoadedMDL();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = HasLoadedSocket(nIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = KModel::Bind(m_pRLScene, m_ModelData.RLMDL.p3DModel, m_ModelData.RLSockets[nIndex].p3DModel, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::UnbindSocket(size_t nIndex)
{
    HRESULT hr = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLSockets.size());
    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KGLOG_PROCESS_ERROR(HasLoadedSocket(nIndex));

    hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, m_ModelData.RLSockets[nIndex].p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KAnimationModel::AppendSlot(LPCTSTR szSocket, IKG3DModel* p3DModel, double fExpireTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLModelSlot* pRLModelSlot = NULL;

    KGLOG_PROCESS_ERROR(p3DModel);
    
    // find empty
    for (pRLModelSlot = m_ModelSlots; pRLModelSlot < m_ModelSlots + _countof(m_ModelSlots); ++pRLModelSlot)
    {
        if (pRLModelSlot->p3DModel == NULL)
        {
            break;
        }
    }

    // not find empty, remove the most early
    if (pRLModelSlot == m_ModelSlots + _countof(m_ModelSlots))
    {
        size_t nMinExpireSlot = 0;

        for (size_t nIndex = 1; nIndex < _countof(m_ModelSlots); ++nIndex)
        {
            if (m_ModelSlots[nIndex].fExpireTime < m_ModelSlots[nMinExpireSlot].fExpireTime)
            {
                nMinExpireSlot = nIndex;
            }
        }

        pRLModelSlot = m_ModelSlots + nMinExpireSlot;
    }

    _tcsncpy(pRLModelSlot->szSocket, szSocket, _countof(pRLModelSlot->szSocket));
    KGLOG_PROCESS_ERROR(pRLModelSlot->szSocket[_countof(pRLModelSlot->szSocket) - 1] != _T('\0'));

    pRLModelSlot->p3DModel = p3DModel;
    pRLModelSlot->fExpireTime = fExpireTime;

    hr = KModel::Bind(m_pRLScene, m_ModelData.RLMDL.p3DModel, pRLModelSlot->p3DModel, pRLModelSlot->szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (pRLModelSlot != m_ModelSlots + _countof(m_ModelSlots))
        {
            memset(pRLModelSlot, 0, sizeof(KRLModelSlot));
        }
        SAFE_RELEASE(p3DModel);
    }
    return hrResult;
}

HRESULT KAnimationModel::ClearSlots()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    for (KRLModelSlot* pRLModelSlot = m_ModelSlots; pRLModelSlot < m_ModelSlots + _countof(m_ModelSlots); ++pRLModelSlot)
    {
        if (pRLModelSlot->p3DModel)
        {
            hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, pRLModelSlot->p3DModel);
            KGLOG_COM_PROCESS_ERROR(hr);

            SAFE_RELEASE(pRLModelSlot->p3DModel);

            memset(pRLModelSlot, 0, sizeof(KRLModelSlot));
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::UpdateSlots()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    for (KRLModelSlot* pRLModelSlot = m_ModelSlots; pRLModelSlot < m_ModelSlots + _countof(m_ModelSlots); ++pRLModelSlot)
    {
        if (pRLModelSlot->p3DModel)
        {
            if (g_pRL->m_fTime > pRLModelSlot->fExpireTime)
            {
                hr = KModel::Unbind(m_pRLScene, m_ModelData.RLMDL.p3DModel, pRLModelSlot->p3DModel);
                KGLOG_COM_PROCESS_ERROR(hr);

                SAFE_RELEASE(pRLModelSlot->p3DModel);

                memset(pRLModelSlot, 0, sizeof(KRLModelSlot));
            }
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetScale(float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = KModel::SetScale(m_ModelData.RLMDL.p3DModel, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    for (size_t nIndex = 0; nIndex < m_ModelData.RLModels.size(); ++nIndex)
    {
        if (m_ModelData.RLModels[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::SetScale(m_ModelData.RLModels[nIndex].p3DModel, fScale);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLSockets.size(); ++nIndex)
    {
        if (m_ModelData.RLSockets[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::SetScale(m_ModelData.RLSockets[nIndex].p3DModel, fScale);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        if (m_ModelData.RLMDLSFXs[nIndex].p3DModel == NULL)
            continue;

        hr = KModel::SetScale(m_ModelData.RLMDLSFXs[nIndex].p3DModel, fScale);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetAlpha(float fAlpha)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = KModel::SetAlpha(m_ModelData.RLMDL.p3DModel, fAlpha);
    KGLOG_COM_PROCESS_ERROR(hr);

    for (size_t nIndex = 0; nIndex < m_ModelData.RLModels.size(); ++nIndex)
    {
        hr = SetModelAlpha(nIndex, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLSockets.size(); ++nIndex)
    {
        hr = SetSocketAlpha(nIndex, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    for (size_t nIndex = 0; nIndex < m_ModelData.RLMDLSFXs.size(); ++nIndex)
    {
        hr = SetMDLSFXAlpha(nIndex, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetModelAlpha(size_t nIndex, float fAlpha)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLModels.size());

    if (m_ModelData.RLModels[nIndex].p3DModel)
    {
        hr = KModel::SetAlpha(m_ModelData.RLModels[nIndex].p3DModel, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetSocketAlpha(size_t nIndex, float fAlpha)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLSockets.size());

    if (m_ModelData.RLSockets[nIndex].p3DModel)
    {
        hr = KModel::SetAlpha(m_ModelData.RLSockets[nIndex].p3DModel, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetMDLSFXAlpha(size_t nIndex, float fAlpha)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    ASSERT(nIndex < m_ModelData.RLMDLSFXs.size());

    if (m_ModelData.RLMDLSFXs[nIndex].p3DModel)
    {
        hr = KModel::SetAlpha(m_ModelData.RLMDLSFXs[nIndex].p3DModel, fAlpha);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetPosition(D3DXVECTOR3 const& vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    m_ModelData.RLMDL.vPosition = vPosition;

    if (m_ModelData.RLMDL.p3DModel && m_ModelData.RLMDL.bVisible)
    {
        hr = KModel::SetPosition(m_ModelData.RLMDL.p3DModel, vPosition);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::GetPosition(D3DXVECTOR3& vPosition) const
{
    vPosition = m_ModelData.RLMDL.vPosition;
    return S_OK;
}

HRESULT KAnimationModel::SetRotation(D3DXQUATERNION const& qRotation)
{
    return KModel::SetRotation(m_ModelData.RLMDL.p3DModel, qRotation);
}

HRESULT KAnimationModel::GetRotation(D3DXQUATERNION& qRotation) const
{
    return KModel::GetRotation(m_ModelData.RLMDL.p3DModel, qRotation);
}

HRESULT KAnimationModel::GetBonePosition(D3DXVECTOR3& vPosition, D3DXVECTOR3 vOffset, LPCTSTR szBone) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BindExtraInfo BindInfo;
    D3DXMATRIX mBone;

    KG_PROCESS_ERROR(szBone);
    KG_PROCESS_ERROR(HasLoadedMDL());

    vPosition = D3DXVECTOR3(0, 0, 0);

    if (_tcslen(szBone) > 2 && szBone[0] == _T('S') && szBone[1] == _T('_'))
    {
        int nSocketIndex = -1;

        nSocketIndex = m_ModelData.RLMDL.p3DModel->FindSocket(szBone, &BindInfo);
        KG_PROCESS_ERROR(nSocketIndex != -1);

        hr = m_ModelData.RLMDL.p3DModel->GetSocketMatrix(&BindInfo, &mBone, &vOffset);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        int nBoneIndex = -1;

        nBoneIndex = m_ModelData.RLMDL.p3DModel->FindBone(szBone, &BindInfo);
        KG_PROCESS_ERROR(nBoneIndex != -1);

        hr = m_ModelData.RLMDL.p3DModel->GetBoneMatrix(&BindInfo, &mBone, &vOffset);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    D3DXVec3TransformCoord(&vPosition, &vPosition, &mBone);    

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("KAnimationModel::GetBonePosition(%s)\n", szBone);
    }
    return hrResult;
}

HRESULT KAnimationModel::GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(!HasLoadedMDL());

    hr = KModel::GetBBox(m_ModelData.RLMDL.p3DModel, vMin, vMax);
    KGLOG_COM_CHECK_ERROR(hr);

Exit1:
    return S_OK;
}

HRESULT KAnimationModel::SetPercentage(DWORD dwColor, float fPercentage)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetPercentage(p3DModel, fPercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = KModel::SetPercentageColor(p3DModel, D3DXCOLOR(dwColor));
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetPercentageVisible(BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetPercentageVisible(p3DModel, bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetCaptionVisible(DWORD dwIndex, BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetCaptionVisible(p3DModel, dwIndex, bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetCaption(DWORD dwIndex, LPCSTR szContent)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetCaption(p3DModel, dwIndex, szContent);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetCaptionColor(DWORD dwIndex, DWORD dwColor)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetCaptionColor(p3DModel, dwIndex, D3DXCOLOR(dwColor));
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetCaptionFloorHeight(float fHeight)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetCaptionFloorHeight(p3DModel, fHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::PauseAnimation(BOOL bPause)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::PauseAnimation(p3DModel, bPause);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::PlayMonoAnimation(KRLMonoAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::PlayMonoAnimation(p3DModel, Param);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::PlaySplitAnimation(KRLSplitAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::PlaySplitAnimation(p3DModel, Param);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SeekMonoAnimation(KRLMonoAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KG_PROCESS_ERROR(Param.szFilePath[0] != _T('\0'));

    hr = KModel::SeekMonoAnimation(m_ModelData.RLMDL.p3DModel, Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("KAnimationModel::SeekMonoAnimation(%s, %d, %f)\n", Param.szFilePath, Param.dwPlayType, Param.fSpeed);
    }
    return hrResult;
}

HRESULT KAnimationModel::SeekSplitAnimation(KRLSplitAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(HasLoadedMDL());
    KG_PROCESS_ERROR(Param.szFilePath[0] != _T('\0'));

    hr = KModel::SeekSplitAnimation(m_ModelData.RLMDL.p3DModel, Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KFilePath::TabError("KAnimationModel::SeekSplitAnimation(%s, %d, %f)\n", Param.szFilePath, Param.dwPlayType, Param.fSpeed);
    }
    return hrResult;
}

HRESULT KAnimationModel::SetMonoAnimationSpeed(float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(HasLoadedMDL());

    hr = KModel::SetMonoAnimationSpeed(m_ModelData.RLMDL.p3DModel, fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetSplitAnimationSpeed(DWORD dwPartIndex, float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(HasLoadedMDL());

    hr = KModel::SetSplitAnimationSpeed(m_ModelData.RLMDL.p3DModel, dwPartIndex, fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::AnimationFinished(IKG3DModel* p3DModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    size_t nIndex = 0;

    if (LookupMDLSFX(p3DModel, nIndex))
    {
        hr = UnloadMDLSFX(nIndex);
        KG_COM_PROCESS_ERROR(hr);
    }
    else if (LookupSocketSFX(p3DModel, nIndex))
    {
        hr = UnloadSocketSFX(nIndex);
        KG_COM_PROCESS_ERROR(hr);
    }
    else if (LookupEquipSFX(p3DModel, nIndex))
    {
        hr = UnloadEquipSFX(nIndex);
        KG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::BindTo(IRLModel* pRLModelTo, LPCTSTR szSocket)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModelTo = NULL;
    KAnimationModel* pAnimationModel = NULL;

    ASSERT(dynamic_cast<KAnimationModel*>(pRLModelTo));

    pAnimationModel = static_cast<KAnimationModel*>(pRLModelTo);

    p3DModelTo = pAnimationModel->m_ModelData.RLMDL.p3DModel;
    KGLOG_PROCESS_ERROR(p3DModelTo);

    hr = BindTo(p3DModelTo, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::UnbindFrom(IRLModel* pRLModelFrom)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModelFrom = NULL;
    KAnimationModel* pAnimationModel = NULL;

    ASSERT(dynamic_cast<KAnimationModel*>(pRLModelFrom));

    pAnimationModel = static_cast<KAnimationModel*>(pRLModelFrom);

    p3DModelFrom = pAnimationModel->m_ModelData.RLMDL.p3DModel;
    KGLOG_PROCESS_ERROR(p3DModelFrom);

    hr = UnbindFrom(p3DModelFrom);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::BindTo(IKG3DModel* p3DModelTo, LPCTSTR szSocket)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);

    p3DModel->ResetTransform();

    hr = KModel::Bind(m_pRLScene, p3DModelTo, p3DModel, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::UnbindFrom(IKG3DModel* p3DModelFrom)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    KGLOG_PROCESS_ERROR(m_pRLScene);

    hr = KModel::Unbind(m_pRLScene, p3DModelFrom, p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::SetBoneTransform(LPCSTR szBone, BoneTransformationInputData const& Data)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::SetBoneTransform(p3DModel, szBone, Data);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KAnimationModel::EnableBoneTransform(BOOL bEnable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IKG3DModel* p3DModel = NULL;

    p3DModel = m_ModelData.RLMDL.p3DModel;
    KG_PROCESS_SUCCESS(p3DModel == NULL);

    hr = KModel::EnableBoneTransform(p3DModel, bEnable);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}
