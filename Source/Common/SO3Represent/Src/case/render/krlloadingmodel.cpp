#include "stdafx.h"
#include "./krlloadingmodel.h"
#include "./kanimationmodel.h"

KRLLoadingModel::KRLLoadingModel()
: m_pRLModel(NULL)
, m_pLoadingRLModel(NULL)
{
}

KRLLoadingModel::~KRLLoadingModel()
{
    ASSERT(m_pRLModel == NULL);
    ASSERT(m_pLoadingRLModel == NULL);
}

HRESULT KRLLoadingModel::Reset()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->Reset();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->Reset();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::Update()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->Update();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->Update();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

KRLScene* KRLLoadingModel::GetRLScene() const
{
    ASSERT(m_pRLModel->GetRLScene() == m_pLoadingRLModel->GetRLScene());

    return m_pRLModel->GetRLScene();
}

HRESULT KRLLoadingModel::SetMDLSelectable(BOOL bSelectable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMDLSelectable(bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMDLSelectable(bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetMDLVisible(BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMDLVisible(bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMDLVisible(bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetMDLSFXInfo(size_t nIndex, BOOL bBind)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMDLSFXInfo(nIndex, bBind);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMDLSFXInfo(nIndex, bBind);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SeekMonoAnimation(KRLMonoAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SeekMonoAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SeekMonoAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SeekSplitAnimation(KRLSplitAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SeekSplitAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SeekSplitAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::AnimationFinished(IKG3DModel* p3DModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->AnimationFinished(p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->AnimationFinished(p3DModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadMDL(LPCTSTR szFilePath, float fScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->LoadMDL(szFilePath, fScale, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadMDL()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->UnloadMDL();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadModel(size_t nIndex, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->LoadModel(nIndex, szMesh, szMaterial, fScale, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadModel(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->UnloadModel(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadSocket(size_t nIndex, LPCTSTR szSocket, LPCTSTR szMesh, LPCTSTR szMaterial, float fScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->LoadSocket(nIndex, szSocket, szMesh, szMaterial, fScale, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadSocket(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->UnloadSocket(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadMDLSFX(size_t nIndex, KRLSFXParam const& SFXParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->LoadMDLSFX(nIndex, SFXParam);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->LoadMDLSFX(nIndex, SFXParam);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadMDLSFX(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->UnloadMDLSFX(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnloadMDLSFX(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadEquipSFX(size_t nIndex, LPCTSTR szFilePath, float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->LoadEquipSFX(nIndex, szFilePath, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->LoadEquipSFX(nIndex, szFilePath, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::LoadSocketSFX(size_t nIndex, LPCTSTR szFilePath, float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->LoadSocketSFX(nIndex, szFilePath, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->LoadSocketSFX(nIndex, szFilePath, fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadAllModel()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->UnloadAllModel();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadAllSocket()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pLoadingRLModel->UnloadAllSocket();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadAllMDLSFX()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->UnloadAllMDLSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnloadAllMDLSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadAllEquipSFX()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->UnloadAllEquipSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnloadAllEquipSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnloadAllSocketSFX()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->UnloadAllSocketSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnloadAllSocketSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLLoadingModel::IsReady() const
{
    return m_pLoadingRLModel->IsReady();
}

BOOL KRLLoadingModel::HasLoadedMDL() const
{
    return m_pLoadingRLModel->HasLoadedMDL();
}

BOOL KRLLoadingModel::HasLoadedModel(size_t nIndex) const
{
    return m_pLoadingRLModel->HasLoadedModel(nIndex);
}

BOOL KRLLoadingModel::HasLoadedSocket(size_t nIndex) const
{
    return m_pLoadingRLModel->HasLoadedSocket(nIndex);
}

BOOL KRLLoadingModel::HasLoadedMDLSFX(size_t nIndex) const
{
    return m_pLoadingRLModel->HasLoadedMDLSFX(nIndex);
}

BOOL KRLLoadingModel::HasLoadedEquipSFX(size_t nIndex) const
{
    return m_pLoadingRLModel->HasLoadedEquipSFX(nIndex);
}

BOOL KRLLoadingModel::HasLoadedSocketSFX(size_t nIndex) const
{
    return m_pLoadingRLModel->HasLoadedSocketSFX(nIndex);
}

BOOL KRLLoadingModel::IsVisible() const
{
    return m_pRLModel->IsVisible();
}

BOOL KRLLoadingModel::IsMDLSelectable() const
{
    return m_pRLModel->IsMDLSelectable();
}

HRESULT KRLLoadingModel::SetMDLSFXPosition(size_t nIndex, D3DXVECTOR3 vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMDLSFXPosition(nIndex, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMDLSFXPosition(nIndex, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetMDLSFXOffsetDirection(size_t nIndex, D3DXVECTOR3 vOffset)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMDLSFXOffsetDirection(nIndex, vOffset);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMDLSFXOffsetDirection(nIndex, vOffset);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::BindSocket(size_t nIndex, LPCTSTR szSocket)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->BindSocket(nIndex, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->BindSocket(nIndex, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnbindSocket(size_t nIndex)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->UnbindSocket(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnbindSocket(nIndex);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::AppendSlot(LPCTSTR szSocket, IKG3DModel* p3DModel, double fExpireTime)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->AppendSlot(szSocket, p3DModel, fExpireTime);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::ClearSlots()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->ClearSlots();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetScale(float fScale)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetScale(fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetScale(fScale);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetAlpha(float fAlpha)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetAlpha(fAlpha);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetAlpha(fAlpha);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetPosition(D3DXVECTOR3 const& vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::GetPosition(D3DXVECTOR3& vPosition) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->GetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->GetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetRotation(D3DXQUATERNION const& qRotation)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetRotation(qRotation);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetRotation(qRotation);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::GetRotation(D3DXQUATERNION& qRotation) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->GetRotation(qRotation);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->GetRotation(qRotation);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::GetBonePosition(D3DXVECTOR3& vPosition, D3DXVECTOR3 vOffset, LPCTSTR szBone) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->GetBonePosition(vPosition, vOffset, szBone);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->GetBBox(vMin, vMax);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetPercentageVisible(BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetPercentageVisible(bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetPercentageVisible(bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetPercentage(DWORD dwColor, float fPercentage)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetPercentage(dwColor, fPercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetPercentage(dwColor, fPercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetCaptionVisible(DWORD dwIndex, BOOL bVisible)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetCaptionVisible(dwIndex, bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetCaptionVisible(dwIndex, bVisible);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetCaption(DWORD dwIndex, LPCSTR szContent)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetCaption(dwIndex, szContent);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetCaption(dwIndex, szContent);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetCaptionColor(DWORD dwIndex, DWORD dwColor)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetCaptionColor(dwIndex, dwColor);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetCaptionColor(dwIndex, dwColor);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetCaptionFloorHeight(float fHeight)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetCaptionFloorHeight(fHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetCaptionFloorHeight(fHeight);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::PauseAnimation(BOOL bPause)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->PauseAnimation(bPause);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->PauseAnimation(bPause);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::PlayMonoAnimation(KRLMonoAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->PlayMonoAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->PlayMonoAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::PlaySplitAnimation(KRLSplitAnimationParam const& Param)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->PlaySplitAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->PlaySplitAnimation(Param);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetMonoAnimationSpeed(float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetMonoAnimationSpeed(fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetMonoAnimationSpeed(fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetSplitAnimationSpeed(DWORD dwPartIndex, float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetSplitAnimationSpeed(dwPartIndex, fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetSplitAnimationSpeed(dwPartIndex, fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::BindTo(IRLModel* pRLModel, LPCTSTR szSocket)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLLoadingModel* pRLLoadingModel = NULL;

    ASSERT(dynamic_cast<KRLLoadingModel*>(pRLModel));

    pRLLoadingModel = static_cast<KRLLoadingModel*>(pRLModel);

    hr = m_pRLModel->BindTo(pRLLoadingModel->m_pRLModel, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->BindTo(pRLLoadingModel->m_pLoadingRLModel, szSocket);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::UnbindFrom(IRLModel* pRLModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLLoadingModel* pRLLoadingModel = NULL;

    ASSERT(dynamic_cast<KRLLoadingModel*>(pRLModel));

    pRLLoadingModel = static_cast<KRLLoadingModel*>(pRLModel);

    hr = m_pRLModel->UnbindFrom(pRLLoadingModel->m_pRLModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->UnbindFrom(pRLLoadingModel->m_pLoadingRLModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::SetBoneTransform(LPCSTR szBone, BoneTransformationInputData const& Data)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->SetBoneTransform(szBone, Data);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->SetBoneTransform(szBone, Data);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLLoadingModel::EnableBoneTransform(BOOL bEnable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_pRLModel->EnableBoneTransform(bEnable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pLoadingRLModel->EnableBoneTransform(bEnable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}
