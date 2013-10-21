#include "stdafx.h"
#include "IKG3DSoundShell.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./krlcharacter.h"
#include "./krlmissile.h"
#include "./krlrides.h"
#include "./krlscene.h"
#include "./krlcamera.h"
#include "./krlremotecharacter.h"
#include "../gameworld/ktabletransform.h"
#include "../gameworld/krlanimationtable.h"
#include "../render/kmodel.h"
#include "../../common/kmathtools.h"
#include "../../SO3Represent.h"
#include "../../../SO3World/KPlayer.h"

#pragma warning(push)
#pragma warning(disable : 4355) // warning C4355: 'this' : used in base member initializer list

namespace
{
    int const s_aModelPlayerSFX[SKILL_BUFF_PART_COUNT][2] =
    {   
        // Left                                 // Right
        {CHARACTER_SFX_SKILL_BUFF_TOP_LH,       CHARACTER_SFX_SKILL_BUFF_TOP_RH},       // SKILL_BUFF_PART_TOP
        {CHARACTER_SFX_SKILL_BUFF_MIDDLE_LH,    CHARACTER_SFX_SKILL_BUFF_MIDDLE_RH},    // SKILL_BUFF_PART_MIDDLE
        {CHARACTER_SFX_SKILL_BUFF_BOTTOM_LH,    CHARACTER_SFX_SKILL_BUFF_BOTTOM_RH}     // SKILL_BUFF_PART_BOTTOM
    };

    void ResetRLAnimationCommonInfo(KRLAnimationCommonInfo& Info)
    {
        Info.dwAnimationID = 0;
        Info.dwKindID = 0;
        Info.dwTrackID = 0;
    }
}

KRLCharacter::KRLCharacter()
: m_pRLHorse(NULL)
, m_pRLVehicle(NULL)
, m_dwTitleMarkEffect(0)
, m_dwSpeechID(0)
, m_bAnimationPaused(FALSE)
, m_bSheath(TRUE)
, m_nUpBodyState(RL_ANIMATION_UP_NONE)
, m_nDownBodyState(RL_ANIMATION_DOWN_NONE)
, m_dwSerialAnimationID(0)
, m_fRideAdjustHeight(0.0f)
, m_nVehicleTrack(0)
{
    memset(&m_AnimationStatus, 0, sizeof(m_AnimationStatus));
    m_AnimationStatus.nAdvance = RL_ADVANCE_FORWARD;

    m_RenderData.Init(this, CHARACTER_EQUIP_COUNT, CHARACTER_SOCKET_COUNT, CHARACTER_SFX_COUNT);

    memset(&m_EquipResource, 0, sizeof(m_EquipResource));

    memset(&m_RLControl, 0, sizeof(m_RLControl));

    memset(&m_UpBodyHighAnimationTrack, 0, sizeof(m_UpBodyHighAnimationTrack));
    memset(&m_UpBodyLowAnimationTrack, 0, sizeof(m_UpBodyLowAnimationTrack));

    memset(&m_DownBodyHighAnimationTrack, 0, sizeof(m_DownBodyHighAnimationTrack));
    memset(&m_DownBodyLowAnimationTrack, 0, sizeof(m_DownBodyLowAnimationTrack));

    m_pUpBodyCurrentAnimationTrack = &m_UpBodyLowAnimationTrack;
    m_pDownBodyCurrentAnimationTrack = &m_DownBodyLowAnimationTrack;

    m_nFightFlag = RL_FIGHT_FLAG_NONE;
    memset(m_dwSkillBuffs, 0, sizeof(m_dwSkillBuffs));
}

KRLCharacter::~KRLCharacter()
{
    Reset();
}

int KRLCharacter::Init(KCharacter* pCharacter, KRLScene* pRLScene)
{
    int nRetCode = false;
    int nResult = false;

    ASSERT(pCharacter);
    ASSERT(pRLScene);

    m_bCurrentPlayer = g_pRL->m_pSO3World->m_dwClientPlayerID == pCharacter->m_dwID;

    pCharacter->m_pRepresentObject = reinterpret_cast<void*>(this);
    m_RenderData.AttachScene(pRLScene);
    m_RenderData.SetMDLSFXInfo(CHARACTER_SFX_LIFE, IS_PLAYER(pCharacter->m_dwID));
    m_RenderData.SetMDLSFXInfo(CHARACTER_SFX_TITLE_MARK, IS_PLAYER(pCharacter->m_dwID));

    m_FrameData.Init(pCharacter);

    nRetCode = m_FrameData.InvalidateFrameData(
        g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop - 1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_FrameData.UpdateFrameData(
        g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, m_RenderData.IsVisible());
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = InitSkillSequence(&m_SkillSequence);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::LoadRides(DWORD dwRepresentID, KRLRides** ppRLRides)
{  
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    DWORD dwObjectID = GetObjectID();
    KRidesModel const* pModel = NULL;
    KRLRides* pRLRides = NULL;
    int nAppendRidesMgr = false;

    ASSERT(ppRLRides);
    ASSERT(*ppRLRides == NULL);

    pModel = g_pRL->m_TableList.GetRidesModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pModel);

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    pRLRides = pRLScene->m_RidesGCMgr.Lookup(dwObjectID);
    if (pRLRides)
    {
        hr = pRLScene->m_RidesGCMgr.Remove(dwObjectID);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        pRLRides = new(std::nothrow) KRLRides;
        KGLOG_PROCESS_ERROR(pRLRides);
    }

    hr = pRLScene->m_RidesMgr.Append(dwObjectID, pRLRides);
    KGLOG_COM_PROCESS_ERROR(hr);

    nAppendRidesMgr = true;

    hr = m_RenderData.SetMDLVisible(FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLRides->AttachObject(dwObjectID);
	pRLRides->SetObjectType(RL_OBJECT_RIDES);

    pRLRides->m_pRLCharacter = this;
    pRLRides->m_dwRepresentID = dwRepresentID;
    pRLRides->m_dwIdleAnimationID = pModel->dwIdleAnimationID;
    pRLRides->m_dwSlopeType = pModel->nSlopeType;
    
    nRetCode = pRLRides->Init(pRLScene, m_bCurrentPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = pRLRides->DisplayDataChanged();
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLRides->SetAlpha(1.0f); 
    pRLRides->FadeIn();

    m_fRideAdjustHeight = pModel->fCameraAdjust;

    *ppRLRides = pRLRides;
    pRLRides = NULL;

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_DELETE(pRLRides);

        if (nAppendRidesMgr)
        {
            hr = pRLScene->m_RidesMgr.Remove(dwObjectID);
            KGLOG_COM_CHECK_ERROR(hr);

			nAppendRidesMgr = false;
        }
    }
    return nResult;
}

int KRLCharacter::UnloadRides(KRLRides** ppRLRides)
{   
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    KRLRides* pRLRides = NULL;
    DWORD dwObjectID = 0;
    int nAppendRidesGCMgr = false;

    KGLOG_PROCESS_ERROR(ppRLRides);
    KGLOG_PROCESS_ERROR(*ppRLRides);

    pRLRides = *ppRLRides;
    *ppRLRides = NULL;

    dwObjectID = GetObjectID();

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    hr = pRLScene->m_RidesMgr.Remove(dwObjectID);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pRLScene->m_RidesGCMgr.Append(dwObjectID, pRLRides);
    KGLOG_COM_PROCESS_ERROR(hr);

    nAppendRidesGCMgr = true;

    hr = pRLRides->UnbindCharacter();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.SetMDLVisible(TRUE);
    KGLOG_COM_CHECK_ERROR(hr);

    pRLRides->SetAlpha(0.0f);
    pRLRides->FadeOut();

    pRLRides->m_pRLCharacter = NULL;
    pRLRides = NULL;

    m_fRideAdjustHeight = 0.0f;

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_DELETE(pRLRides);

        if (nAppendRidesGCMgr)
        {
            hr = pRLScene->m_RidesGCMgr.Remove(dwObjectID);
            KGLOG_COM_CHECK_ERROR(hr);

			nAppendRidesGCMgr = false;
        }
    }
    return nResult;
}

void KRLCharacter::UpdateWeaponSocketPosition(BOOL bSheath)
{
    if (m_RLControl.pUpdateWeapon)
    {
        (this->*m_RLControl.pUpdateWeapon)(bSheath);
    }
}

HRESULT KRLCharacter::LoadTitleMark(DWORD dwEffect)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KGlobalEffectModel const* pGlobalEffect = NULL;
    KRLSFXParam SFXParam;

    pGlobalEffect = g_pRL->m_TableList.GetGlobalEffectModel(dwEffect);
    KGLOG_PROCESS_ERROR(pGlobalEffect);

    SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pGlobalEffect->dwPlayType);
    SFXParam.fScale = pGlobalEffect->fScale;
    SFXParam.fSpeed = pGlobalEffect->fSpeed;
    SFXParam.szBoneName = _T("");
    SFXParam.szFilePath = pGlobalEffect->szFilePath;

    if (SFXParam.szFilePath[0] != _T('\0'))
    {
        hr = m_RenderData.LoadMDLSFX(CHARACTER_SFX_TITLE_MARK, SFXParam);
        KGLOG_COM_PROCESS_ERROR(hr);

        m_dwTitleMarkEffect = dwEffect;
    }

    UpdateTitleMarkPosition();

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (m_dwTitleMarkEffect != 0)
        {
            m_dwTitleMarkEffect = 0;

            hr = m_RenderData.UnloadMDLSFX(CHARACTER_SFX_TITLE_MARK);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KRLCharacter::UnloadTitleMark()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    m_dwTitleMarkEffect = 0;

    if (m_RenderData.HasLoadedMDLSFX(CHARACTER_SFX_TITLE_MARK))
    {
        hr = m_RenderData.UnloadMDLSFX(CHARACTER_SFX_TITLE_MARK);
        KGLOG_COM_PROCESS_ERROR(hr);

        nRetCode = UpdateTitleMarkPosition();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLCharacter::UpdateSkillBuff(KSkillBuffParam const* pSkillBuff, size_t nSkillBuffCount)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pSkillBuff);

    ASSERT(nSkillBuffCount == SKILL_BUFF_PART_COUNT);

    for (size_t nIndex = 0; nIndex < nSkillBuffCount; ++nIndex)
    {
        if (pSkillBuff[nIndex].SFXParam.szFilePath[0] != _T('\0'))
        {
            if (m_pRLHorse && pSkillBuff[nIndex].szBoneNameLeft[0] == _T('\0') && pSkillBuff[nIndex].szBoneNameRight[0] == _T('\0'))
            {
                m_pRLHorse->LoadSkillBuff(nIndex, pSkillBuff[nIndex]);
            }
            else
            {
                LoadSkillBuff(nIndex, pSkillBuff[nIndex]);
            }
        }
        else
        {
            if (m_pRLHorse)
            {
                m_pRLHorse->UnloadSkillBuff(nIndex);
            }

            UnloadSkillBuff(nIndex);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::LoadSkillBuff(size_t nIndex, KSkillBuffParam SkillBuff)
{
    HRESULT hr = E_FAIL;

    if (SkillBuff.szBoneNameLeft[0] == _T('\0') && SkillBuff.szBoneNameRight[0] == _T('\0'))
    {  
        // 绑到原点
        hr = m_RenderData.LoadMDLSFX(s_aModelPlayerSFX[nIndex][0], SkillBuff.SFXParam);
        KGLOG_COM_CHECK_ERROR(hr);
    }
    else
    {
        if (SkillBuff.szBoneNameLeft[0] != _T('\0'))
        { 
            // Left
            SkillBuff.SFXParam.szBoneName = SkillBuff.szBoneNameLeft;

            hr = m_RenderData.LoadMDLSFX(s_aModelPlayerSFX[nIndex][0], SkillBuff.SFXParam);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (SkillBuff.szBoneNameRight[0] != _T('\0'))
        {
            // Right
            SkillBuff.SFXParam.szBoneName = SkillBuff.szBoneNameRight;

            hr = m_RenderData.LoadMDLSFX(s_aModelPlayerSFX[nIndex][1], SkillBuff.SFXParam);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KRLCharacter::UnloadSkillBuff(size_t nIndex)
{
    HRESULT hr = E_FAIL;

    if (m_RenderData.HasLoadedMDLSFX(s_aModelPlayerSFX[nIndex][0]))
    {
        hr = m_RenderData.UnloadMDLSFX(s_aModelPlayerSFX[nIndex][0]);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    if (m_RenderData.HasLoadedMDLSFX(s_aModelPlayerSFX[nIndex][1]))
    {
        hr = m_RenderData.UnloadMDLSFX(s_aModelPlayerSFX[nIndex][1]);
        KGLOG_COM_CHECK_ERROR(hr);
    }
}

int KRLCharacter::PlaySound(int nType, LPCTSTR pszFilePath, float fMinDistance, float fMaxDistance, PDWORD pdwSoundID)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vPos;
    D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);
    IKG3DSoundShell* pSoundShell = g_pRL->m_pSound;

    KG_PROCESS_SUCCESS(pSoundShell == NULL);

    GetPosition(vPos);

    vPos *= 0.001f;

    hr = pSoundShell->Play3DSound(nType, 
        pszFilePath, 
        1.0f, FALSE, &vPos, &vVel, 
        fMinDistance, 
        fMaxDistance, 
        pdwSoundID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KFilePath::TabError("KRLCharacter::PlaySound(%s, %g, %g)\n", pszFilePath, fMinDistance, fMaxDistance);
    }
    return nResult;
}

int KRLCharacter::StopSound(DWORD dwSoundID)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = g_pRL->m_pSound;

    KG_PROCESS_SUCCESS(pSoundShell == NULL);

    hr = pSoundShell->Stop3DSound(dwSoundID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::PlaySpeech(DWORD dwTarget, BOOL bTurnToTarget, LPCTSTR pszFilePath)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = PlaySound(CHARACTER_SPEAK, pszFilePath, 2500.0f, 3000.0f, &m_dwSpeechID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::StopSpeech()
{
    int nRetCode = false;
    int nResult = false;

    KG_PROCESS_SUCCESS(m_dwSpeechID == 0);

    nRetCode = StopSound(m_dwSpeechID);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_dwSpeechID = 0;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::PlaySound(DWORD dwSoundID)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLSoundModel const* pRLSoundModel = NULL;

    pRLSoundModel = g_pRL->m_TableList.GetSoundModel(dwSoundID);
    KGLOG_PROCESS_ERROR(pRLSoundModel);

    nRetCode = PlaySound(
        CHARACTER_SOUND,
        pRLSoundModel->szSoundFile, 
        pRLSoundModel->fSoundMinRange, 
        pRLSoundModel->fSoundMaxRange, 
        NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pRLSoundModel)
        {
            KGLogPrintf(KGLOG_ERR, "KRLCharacter::PlaySound(%s)\n", pRLSoundModel->szSoundFile);
        }
    }
    return nResult;
}

int KRLCharacter::PlayLevelUpSFX(REPRESENT_SFX nSFX)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLSFXParam SFXParam;

    KG_PROCESS_SUCCESS(!IS_PLAYER(GetObjectID()));

    nRetCode = GetCharacterLevelUpSFX(m_EquipResource, nSFX, &SFXParam);
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(SFXParam.szFilePath[0] != _T('\0'));

    hr = m_RenderData.LoadMDLSFX(CHARACTER_SFX_SECONDARY, SFXParam);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::PlayAnimation(DWORD dwAnimationID, RL_ANIMATION_STATE nState, RL_ANIMATION_UP nUpState, RL_ANIMATION_DOWN nDownState, int nOffsetTime, void* pUserdata)
{
    int nRetCode = false;
    int nResult = false;
    KRLAniInfo AniInfo;
    KRLAdjustAnimationParam Param;

    KG_PROCESS_SUCCESS(m_bAnimationPaused);

    nRetCode = FillRLAdjustAnimationParam(&Param, m_FrameData.m_Current.bSheathFlag);
    KGLOG_PROCESS_ERROR(nRetCode);

    AniInfo.dwAnimationID = GetAdjustAnimationIndexID(dwAnimationID, &Param);
    AniInfo.nState = nState;
    AniInfo.nUpState = nUpState;
    AniInfo.nDownState = nDownState;
    AniInfo.nOffsetTime = nOffsetTime;
    AniInfo.pUserdata = pUserdata;

    if (m_RLControl.pPlayAnimation)
        (this->*m_RLControl.pPlayAnimation)(AniInfo);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::CastSkill(KSkillCasterParam const& SkillCasterParam, DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwAnimationID = 0;
    KRLSkillSequnceParam SkillSequenceParam;

    KGLOG_PROCESS_ERROR(m_RenderData.HasLoadedMDL());

    SkillSequenceParam.dwSkillID = dwSkillID;
    SkillSequenceParam.dwSkillLevel = dwSkillLevel;
    memcpy(SkillSequenceParam.dwAnimationIDs, SkillCasterParam.dwCastSkillAnimationIDs, sizeof(SkillSequenceParam.dwAnimationIDs));
    
    nRetCode = GetSkillNextAnimationID(&m_SkillSequence, &SkillSequenceParam, &dwAnimationID);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_FrameData.UpdateCurrentOTAction();

    if (m_nUpBodyState == RL_ANIMATION_UP_SKILL_CHANNEL && m_FrameData.m_Current.nOTAction != otActionSkillChannel ||
        m_nUpBodyState == RL_ANIMATION_UP_SKILL_PREPARE && m_FrameData.m_Current.nOTAction != otActionSkillPrepare ||
        m_dwSerialAnimationID != 0)
    {
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_NONE, RL_ANIMATION_DOWN_UNKNOWN, 0, (void*)(UINT_PTR)dwBulletID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::BeginPlaySerialAnimation(DWORD dwSerialID)
{
    int nRetCode = false;
    int nResult = false;
    KRLSerialAnimation SerialAnimation;

    nRetCode = GetCharacterSerialAnimation(m_EquipResource.nObject == RL_OBJECT_PLAYER, dwSerialID, &SerialAnimation);
    KG_PROCESS_ERROR(nRetCode);

    if (SerialAnimation.dwPhaseAnimationIDs[0])         // RL_ANIMATION_UP_SERIAL_BEGIN
    {
        m_dwSerialAnimationID = dwSerialID;

        nRetCode = PlayAnimation(SerialAnimation.dwPhaseAnimationIDs[0], 
            RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_SERIAL_BEGIN, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else if (SerialAnimation.dwPhaseAnimationIDs[1])    // RL_ANIMATION_UP_SERIAL_PROCESSING
    {
        m_dwSerialAnimationID = dwSerialID;

        nRetCode = PlayAnimation(SerialAnimation.dwPhaseAnimationIDs[1], 
            RL_ANIMATION_STATE_HOLD_ONCE, RL_ANIMATION_UP_SERIAL_PROCESSING, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::EndPlaySerialAnimation(DWORD dwSerialID)
{
    int nRetCode = false;
    int nResult = false;
    KRLSerialAnimation SerialAnimation;

    nRetCode = GetCharacterSerialAnimation(m_EquipResource.nObject == RL_OBJECT_PLAYER, dwSerialID, &SerialAnimation);
    KG_PROCESS_ERROR(nRetCode);

    if (SerialAnimation.dwPhaseAnimationIDs[2])       // RL_ANIMATION_UP_SERIAL_END
    {
        m_dwSerialAnimationID = dwSerialID;

        nRetCode = PlayAnimation(SerialAnimation.dwPhaseAnimationIDs[2], 
            RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_SERIAL_END, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::UpdateFrameData()
{
    m_FrameData.UpdateFrameData(g_pRL->m_pSO3World->m_nGameLoop - g_pRL->m_nStartLoop, m_RenderData.IsVisible());
}

float KRLCharacter::GetRideAdjustHeight() const
{
    return m_fRideAdjustHeight;
}

int KRLCharacter::Reset()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

    if (m_pRLHorse)
    {
        nRetCode = UnloadRides(&m_pRLHorse);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_pRLVehicle)
    {
        nRetCode = UnloadRides(&m_pRLVehicle);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    hr = m_RenderData.Reset();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.DetachScene();
    KGLOG_COM_PROCESS_ERROR(hr);

    m_FrameData.m_pCharacter = NULL;

    m_nVehicleTrack = 0;

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCharacter::OnPositionChange(D3DXVECTOR3 const& vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pRLHorse)
        m_pRLHorse->SetPosition(vPosition);

    if (m_pRLVehicle)
        m_pRLVehicle->SetPosition(vPosition);

    hr = m_RenderData.SetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLCharacter::OnRotationLHChange(D3DXQUATERNION const& qRotationLH)
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.SetRotation(qRotationLH);
    return S_OK;
}

HRESULT KRLCharacter::OnAlphaChange(float fAlpha)
{
    if (m_pRLHorse)
        m_pRLHorse->SetAlpha(fAlpha);

    if (m_pRLVehicle)
        m_pRLVehicle->SetAlpha(fAlpha);

    if (m_RenderData.HasLoadedMDL())
        m_RenderData.SetAlpha(fAlpha);

    return S_OK;
}

HRESULT KRLCharacter::OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.GetBBox(vMin, vMax);
    return S_OK;
}

HRESULT KRLCharacter::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    int nRetCode = false;
    KRLScene* pRLScene = NULL;

    ASSERT(pController);
    ASSERT(pMotionCallBackExtraInfo);

    KG_PROCESS_SUCCESS(IsFadeOut());

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    switch (pMotionCallBackExtraInfo->Type)
    {
    case MTBT_DAMAGEPERCENTAGE:
        {
            TagMotionCBExtraInfoDamagePercentage* pTagMotionCBExtraInfoDamagePercentage = NULL;
            DWORD dwBulletID = 0;
            float fDamagePercentage = 0.0f;
            
            pTagMotionCBExtraInfoDamagePercentage = reinterpret_cast<TagMotionCBExtraInfoDamagePercentage*>(pMotionCallBackExtraInfo);

            dwBulletID = (DWORD)(UINT_PTR)pController->GetUserdata();
            fDamagePercentage = pTagMotionCBExtraInfoDamagePercentage->fDamagePercentage;

            pRLScene->m_SkillEffectResult.InvokeResult(GetObjectID(), dwBulletID, fDamagePercentage);
        }
        break;
    case MTBT_METEOR:
        break;
    case MTBT_FACEMOTION:
        break;
    case MTBT_MISSILE:
        {
            TagMoitonCBExtraInfoMissile* pTagMoitonCBExtraInfoMissile = NULL;
            DWORD dwMissileID = 0;
            DWORD dwBulletID = 0;
            KRLMissileTagParam TagParam;
            float fDamagePercentage = 0.0f;

            pTagMoitonCBExtraInfoMissile = reinterpret_cast<TagMoitonCBExtraInfoMissile*>(pMotionCallBackExtraInfo);

            dwMissileID = pTagMoitonCBExtraInfoMissile->dwMissileID;
            dwBulletID = (DWORD)(UINT_PTR)pController->GetUserdata();
            TagParam.fPercent = pTagMoitonCBExtraInfoMissile->fDamagePercentage;

            // TODO:
            GetPosition(TagParam.vInitialWorldPosition);
            D3DXQuaternionIdentity(&TagParam.qInitialWorldRotation);

            nRetCode = PrepareAnimationTagMissileParam(this, dwBulletID, dwMissileID, &TagParam);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        break;
    default:
        break;
    }

Exit1:
Exit0:
    return S_OK;
}

HRESULT KRLCharacter::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{   
    return S_OK;
}

HRESULT KRLCharacter::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_SUCCESS(IsFadeOut());
    KG_PROCESS_ERROR(pController);

    switch (pController->GetMotionExtraInfo())
    {
    case IKG3DAnimationController::TYPE_NONE:
        break;
    case IKG3DAnimationController::TYPE_PART0:
        ASSERT(m_EquipResource.bSplitModel);

        if (pController->GetPriority() == ANICTL_SECONDARY)
            UpBodyHighTrackAnimationFinish();
        else
            UpBodyLowTrackAnimationFinish();
        break;
    case IKG3DAnimationController::TYPE_PART1:
        ASSERT(m_EquipResource.bSplitModel);

        if (pController->GetPriority() == ANICTL_SECONDARY)
            DownBodyHighTrackAnimationFinish();
        else
            DownBodyLowTrackAnimationFinish();
        break;
    case IKG3DAnimationController::TYPE_ALLPART:
//        ASSERT(!m_EquipResource.bSplitModel);

        if (pController->GetPriority() == ANICTL_SECONDARY)
            WholeBodyHighTrackAnimationFinish();
        else
            WholeBodyLowTrackAnimationFinish();
        break;
	case IKG3DAnimationController::TYPE_SFX:
        if (pController->GetModel())
        {
            hr = m_RenderData.AnimationFinished(pController->GetModel());
            KGLOG_COM_CHECK_ERROR(hr);
        }
		break;
    default:
        ASSERT(0);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLCharacter::UpdateAnimationDown(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag,
                                      DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT])
{
    int nRetCode = false;
    int nResult = false;
    RL_ANIMATION_STATE nAnimationState = RL_ANIMATION_STATE_HOLD_ONCE;

    if (nAnimationDown == RL_ANIMATION_DOWN_STAND)
    {
        if (m_EquipResource.bSplitModel)
        {
            if (m_nUpBodyState == RL_ANIMATION_UP_SKILL_CHANNEL && m_FrameData.m_Current.nOTAction != otActionSkillChannel ||
                m_nUpBodyState == RL_ANIMATION_UP_SKILL_PREPARE && m_FrameData.m_Current.nOTAction != otActionSkillPrepare ||
                m_nFightFlag != nFightFlag)
            {
                nRetCode = ResetAnimation();
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
        else
        {
            if (m_nFightFlag != nFightFlag)
            {
                nRetCode = WholeBodyLowTrackAnimationFinish();
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            if (m_nUpBodyState == RL_ANIMATION_UP_NONE)
            {
                // npc idle 的时候循环播放
                nAnimationState = RL_ANIMATION_STATE_HOLD_LOOP;
            }
        }
    }

    // Water fly patch
    if ((nAnimationDown == RL_ANIMATION_DOWN_JUMP || nAnimationDown == RL_ANIMATION_DOWN_DOUBLE_JUMP) && 
        (m_nDownBodyState == RL_ANIMATION_DOWN_JUMP || m_nDownBodyState == RL_ANIMATION_DOWN_DOUBLE_JUMP) &&
        m_FrameData.m_Current.bAboveWater && 
        m_FrameData.m_Current.nZ > m_FrameData.m_Previous.nZ && 
        m_FrameData.m_Current.nZ * m_FrameData.m_Previous.nZ < 0)
    {
        m_nDownBodyState = RL_ANIMATION_DOWN_UNKNOWN;
    }
    
    // Freeze
    if (nAnimationDown == RL_ANIMATION_DOWN_FREEZE)
    {
        nRetCode = PauseAnimation(TRUE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = UpdateAnimation(nAnimationDown, nAnimationState, nAdvance, nFightFlag, dwSkillBuffs);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpdateAnimationUp(RL_ANIMATION_UP nAnimationUp)
{
    int nRetCode = false;
    int nResult = false;

    m_nUpBodyState = nAnimationUp;

    switch (nAnimationUp)
    {
    case RL_ANIMATION_UP_NONE:
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_UP_SKILL_PREPARE:
    case RL_ANIMATION_UP_SKILL_CHANNEL:
    case RL_ANIMATION_UP_RECIPE_PREPARE:
    case RL_ANIMATION_UP_PICK_PREPARE:
    case RL_ANIMATION_UP_OPEN_DOODAD:
    case RL_ANIMATION_UP_BEGIN_PICK_DOODAD:
    case RL_ANIMATION_UP_PICKING_DOODAD:
    case RL_ANIMATION_UP_END_PICK_DOODAD:
    case RL_ANIMATION_UP_ITEM_SKILL:
    case RL_ANIMATION_UP_CUSTOM_PREPARE:
    case RL_ANIMATION_UP_CUSTOM_CHANNEL:
    case RL_ANIMATION_UP_BEGIN_SHEATH:
    case RL_ANIMATION_UP_END_SHEATH:
    case RL_ANIMATION_UP_BEGIN_UNSHEATH:
    case RL_ANIMATION_UP_END_UNSHEATH:
    case RL_ANIMATION_UP_DODGE:
    case RL_ANIMATION_UP_BEHIT:
    case RL_ANIMATION_UP_SERIAL_BEGIN:
    case RL_ANIMATION_UP_SERIAL_PROCESSING:
    case RL_ANIMATION_UP_SERIAL_END:
        break;
    default:
        ASSERT(0);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

static int UpdateRemoteVechicleVisible(BOOL bVisible)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        KRLScene* pRLScene = *itScene;

        KRLRemoteCharacterMgr::KPtrMap& Characters = pRLScene->m_CharacterMgr.m_aCharacter;
        for (KRLRemoteCharacterMgr::KPtrMap::iterator it = Characters.begin(); it != Characters.end(); ++it)
        {
            KRLRemoteCharacter* pRLRemoteCharacter = it->second;

            if (pRLRemoteCharacter->m_RLCharacter.m_pRLVehicle == NULL)
                continue;

            hr = pRLRemoteCharacter->m_RLCharacter.m_RenderData.SetMDLVisible(bVisible);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        KRLRemoteCharacterMgr::KPtrMap& GCCharacters = pRLScene->m_CharacterGCMgr.m_aCharacter;
        for (KRLRemoteCharacterMgr::KPtrMap::iterator it = GCCharacters.begin(); it != GCCharacters.end(); ++it)
        {
            KRLRemoteCharacter* pRLRemoteCharacter = it->second;

            if (pRLRemoteCharacter->m_RLCharacter.m_pRLVehicle == NULL)
                continue;

            hr = pRLRemoteCharacter->m_RLCharacter.m_RenderData.SetMDLVisible(bVisible);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

enum RL_VEHICLE_UPDATE_STATE
{
    RL_VEHICLE_UPDATE_STATE_NONE,
    RL_VEHICLE_UPDATE_STATE_LOAD,   // create or change
    RL_VEHICLE_UPDATE_STATE_UNLOAD,
};

int KRLCharacter::UpdateVehicle(int nVehicleTrack, BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
	int nAppendTrackRidesFlag = false;
    DWORD dwVehicleRepresentID = 0;
    DWORD dwAnimationID = 0;
    RL_VEHICLE_UPDATE_STATE nVehicleUpdateState = RL_VEHICLE_UPDATE_STATE_NONE;

    if (nVehicleTrack != 0)
    {
        if (m_pRLVehicle == NULL || bForceUpdate || m_nVehicleTrack != nVehicleTrack)
        {
            nVehicleUpdateState = RL_VEHICLE_UPDATE_STATE_LOAD;
        }
    }
    else
    {
        nVehicleUpdateState = RL_VEHICLE_UPDATE_STATE_UNLOAD;
    }

    KG_PROCESS_SUCCESS(nVehicleUpdateState == RL_VEHICLE_UPDATE_STATE_NONE);

    if (m_pRLVehicle)
    {
        nRetCode = UnloadRides(&m_pRLVehicle);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    KG_PROCESS_SUCCESS(nVehicleUpdateState != RL_VEHICLE_UPDATE_STATE_LOAD);
    KG_PROCESS_SUCCESS(m_EquipResource.nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    dwVehicleRepresentID = g_pRL->m_TableList.GetRidesIDByTrackID((DWORD)nVehicleTrack);

    nRetCode = LoadRides(dwVehicleRepresentID, &m_pRLVehicle);
    KGLOG_PROCESS_ERROR(nRetCode);

    nAppendTrackRidesFlag = true;

    dwAnimationID = m_pRLVehicle->m_dwIdleAnimationID;

    nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_HOLD_ONCE, RL_ANIMATION_UP_NONE, RL_ANIMATION_DOWN_AUTOFLY);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateVehicleVisible();
    KGLOG_PROCESS_ERROR(nRetCode);

    m_nVehicleTrack = nVehicleTrack;

Exit1:
    nResult = true;
Exit0:
	if (!nResult)
	{
		if (nAppendTrackRidesFlag)
		{
			nRetCode = UnloadRides(&m_pRLVehicle);
			KGLOG_CHECK_ERROR(nRetCode);

			nAppendTrackRidesFlag = false;
		}
	}
    return nResult;
}

int KRLCharacter::UpdateVehicleVisible()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    BOOL bVisible = FALSE;

    if (m_bCurrentPlayer)
    {
        // 当自己坐交通工具的时候，隐藏其他坐交通工具的玩家

        bVisible = m_pRLVehicle != NULL;

        UpdateRemoteVechicleVisible(bVisible);
    }
    else
    {
        pRLScene = m_RenderData.GetRLScene();
        KGLOG_PROCESS_ERROR(pRLScene);

        bVisible = pRLScene->m_pRLCamera->m_CharacterCamera.m_RLLocalCharacter.m_RLCharacter.m_pRLVehicle != NULL;

        hr = m_RenderData.SetMDLVisible(bVisible);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpdateNpcModel(DWORD dwAnimationID)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = LoadSocketByAniamtion(dwAnimationID);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::UpdateWeaponPosition(RL_SHEATH_SOCKET nSheathSocket, BOOL bSheath)
{
    switch (nSheathSocket)
    {
    case RL_SHEATH_SOCKET_GAMEWORLD:
        if (m_bSheath != bSheath)
            UpdateWeaponSocketPosition(bSheath);
        break;
    case RL_SHEATH_SOCKET_SHEATH:
        if (!m_bSheath)
            UpdateWeaponSocketPosition(TRUE);
        break;
    case RL_SHEATH_SOCKET_UNSHEATH:
        if (m_bSheath)
            UpdateWeaponSocketPosition(FALSE);
        break;
    default:
        ASSERT(0);
    }
}

void KRLCharacter::UpdateSheath(BOOL bSheathFlag)
{
    if (m_RLControl.pUpdateSheath)
    {
        (this->*m_RLControl.pUpdateSheath)(bSheathFlag);
    }
}

void KRLCharacter::UpdateAnimationSpeed(float fSpeed)
{
    ASSERT(fSpeed > 0.001f && fSpeed < 1000.0f);

    switch (m_AnimationStatus.nType)
    {
    case RL_ANIMATION_NONE:
        break;
    case RL_ANIMATION_MONO:
        m_AnimationStatus.MonoStatus.MonoParam.fSpeed = fSpeed;
        break;
    case RL_ANIMATION_SPLIT:
        m_AnimationStatus.SplitStatus.DownLowTrackParam.fSpeed = fSpeed;
        m_AnimationStatus.SplitStatus.DownHighTrackParam.fSpeed = fSpeed;
        m_AnimationStatus.SplitStatus.UpLowTrackParam.fSpeed = fSpeed;
        m_AnimationStatus.SplitStatus.UpHighTrackParam.fSpeed = fSpeed;
        break;
    default:
        ASSERT(0);
        break;
    }

    if (m_RLControl.pUpdateAnimationSpeed)
    {
        (this->*m_RLControl.pUpdateAnimationSpeed)(fSpeed);
    }
}

int KRLCharacter::UpdateBalloon()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    BOOL bCanSeeName = TRUE;
    BOOL bCanSeeLife = TRUE;
    TCHAR szCaption[MAX_PATH];
    KForceRelationModel const* pForceRelationModel = NULL;
    DWORD dwColor;
    float fScale = 1.0f;
    int nCharacterType = CHARACTER_TYPE_LOCAL_PLAYER;
    float fCaptionFloorSpace = 0.0f;
    KNpc* pNpc = NULL;

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    if (!IS_PLAYER(GetObjectID()))
    {
        pNpc = (KNpc*)m_FrameData.m_pCharacter;
        KGLOG_PROCESS_ERROR(pNpc);
        KGLOG_PROCESS_ERROR(pNpc->m_pTemplate);

        bCanSeeName = pNpc->m_pTemplate->bCanSeeName;
        bCanSeeLife = pNpc->m_pTemplate->bCanSeeLifeBar;
    }

    pForceRelationModel = GetForceRelationModel(GetObjectID(), g_pRL->m_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pForceRelationModel);

    dwColor = pForceRelationModel->dwColor;

    if (!IS_PLAYER(GetObjectID()))
        nCharacterType = CHARACTER_TYPE_NPC;
    else if (m_bCurrentPlayer)
        nCharacterType = CHARACTER_TYPE_LOCAL_PLAYER;
    else
        nCharacterType = CHARACTER_TYPE_REMOTE_PLAYER;

    // 血条显示需要文字先设置才能正常,因此把名字设置提前,因为名字是必定有的,只是是否显示的问题就由下面处理!
    hr = m_RenderData.SetCaption(3, m_FrameData.m_pCharacter->m_szName);
    KGLOG_COM_PROCESS_ERROR(hr);

    // Life
    if (bCanSeeLife && g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][CHARACTER_SFX_LIFE - CHARACTER_SFX_BBOX_BALLOON_BEGIN]) 
    {
        hr = m_RenderData.SetPercentageVisible(TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_RenderData.SetPercentage(dwColor, m_FrameData.m_Current.fLifePercentage);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = m_RenderData.SetPercentageVisible(FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    // Tong
    hr = m_RenderData.SetCaptionVisible(1, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][CHARACTER_SFX_TONG - CHARACTER_SFX_BBOX_BALLOON_BEGIN])
    {
        if (IS_PLAYER(GetObjectID()))
        {
            KPlayer* pPlayer = (KPlayer*)m_FrameData.m_pCharacter;
            if (pPlayer->m_szTongName[0] != '\0')
            {
                _sntprintf(szCaption, _countof(szCaption), _T("<%s>"), pPlayer->m_szTongName);
                szCaption[_countof(szCaption) - 1] = _T('\0');

                hr = m_RenderData.SetCaptionVisible(1, TRUE);
                KGLOG_COM_PROCESS_ERROR(hr);

                hr = m_RenderData.SetCaption(1, szCaption);
                KGLOG_COM_PROCESS_ERROR(hr);

                hr = m_RenderData.SetCaptionColor(1, dwColor);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
            else
            {
                hr = m_RenderData.SetCaptionVisible(1, FALSE);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
        }
    }

    // Title
    if (g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][CHARACTER_SFX_TITLE - CHARACTER_SFX_BBOX_BALLOON_BEGIN])
    {
        if (m_FrameData.m_pCharacter->m_szTitle && m_FrameData.m_pCharacter->m_szTitle[0] != '\0')
        {
            _sntprintf(szCaption, _countof(szCaption), _T("<%s>"), m_FrameData.m_pCharacter->m_szTitle);
            szCaption[_countof(szCaption) - 1] = _T('\0');

            hr = m_RenderData.SetCaptionVisible(2, TRUE);
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = m_RenderData.SetCaption(2, szCaption);
            KGLOG_COM_PROCESS_ERROR(hr);

            hr = m_RenderData.SetCaptionColor(2, dwColor);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
        else
        {
            hr = m_RenderData.SetCaptionVisible(2, FALSE);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }
    else
    {
        hr = m_RenderData.SetCaptionVisible(2, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    // Name
    if (bCanSeeName && g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][CHARACTER_SFX_NAME - CHARACTER_SFX_BBOX_BALLOON_BEGIN])
    {
        hr = m_RenderData.SetCaptionVisible(3, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_RenderData.SetCaptionColor(3, dwColor);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        hr = m_RenderData.SetCaptionVisible(3, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    fCaptionFloorSpace = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAPTION_FLOOR_SPACE);

    hr = m_RenderData.SetCaptionFloorHeight(m_EquipResource.fModelHeight + fCaptionFloorSpace);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::TurnFace(float fDeltaDirection)
{
//    if (abs(fDeltaDirection) > FLT_EPSILON)
    {
        if (m_RLControl.pTurnFaceBegin)
        {
            (this->*m_RLControl.pTurnFaceBegin)(fDeltaDirection);
        }
    }
#if 0
    else
    {
        if (m_RLControl.pTurnFaceEnd)
        {
            (this->*m_RLControl.pTurnFaceEnd)();
        }
    }
#endif
}

int KRLCharacter::UpdateTitleMark()
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    KG_PROCESS_SUCCESS(m_dwTitleMarkEffect == 0);

    hr = LoadTitleMark(m_dwTitleMarkEffect);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpdateTitleMarkPosition()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vPosition;
    float fDelta  = 0.0f;
    float fScale  = 1.0f;    
    float fCaptionFloorSpace = 0.0f;

    KG_PROCESS_SUCCESS(m_dwTitleMarkEffect == 0);

    GetPosition(vPosition);

    fCaptionFloorSpace = g_pRL->m_TableList.GetNumber(RL_NUMBER_CAPTION_FLOOR_SPACE);

    nRetCode = GetCharacterQuestMarkOrSkillBuffHeight(m_FrameData.m_pCharacter, fDelta);
    KGLOG_PROCESS_ERROR(nRetCode);

    if (IS_PLAYER(GetObjectID()))
    {
        memset(vPosition, 0, sizeof(vPosition));
    }

    vPosition.y += m_EquipResource.fModelHeight + fCaptionFloorSpace + fDelta; 

    hr = m_RenderData.SetMDLSFXPosition(CHARACTER_SFX_TITLE_MARK, vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpdatePlayerSheath(BOOL bSheath)
{
    int nRetCode = false;
    int nResult = false;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    DWORD dwAnimationID = 0;

    KG_PROCESS_SUCCESS(m_bSheath == bSheath);

    nAnimationTable = GetSheathAnimationTable(m_EquipResource.Player.nSheathSocket, bSheath, 0);

    if (bSheath)
    {
        dwAnimationID = GetAnimationID(nAnimationTable, FALSE);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_BEGIN_SHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        dwAnimationID = GetAnimationID(nAnimationTable, bSheath);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_BEGIN_UNSHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::UpdatePlayerAnimationSpeed(float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_nUpBodyState == RL_ANIMATION_UP_NONE)
    {
        hr = m_RenderData.SetSplitAnimationSpeed(ANI_JOINT_TOP, fSpeed);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_pDownBodyCurrentAnimationTrack->dwTrackID == ANICTL_PRIMARY)
    {
        hr = m_RenderData.SetSplitAnimationSpeed(ANI_JOINT_BOTTOM, fSpeed);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (m_pRLHorse)
        {
            hr = m_pRLHorse->UpdateAnimationSpeed(fSpeed);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
        if (m_pRLVehicle)
        {
            hr = m_pRLVehicle->UpdateAnimationSpeed(fSpeed);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }
    
    hrResult = S_OK;
Exit0:
    return;
}

void KRLCharacter::UpdateNpcAnimationSpeed(float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_nUpBodyState == RL_ANIMATION_UP_NONE)
    {
        hr = m_RenderData.SetMonoAnimationSpeed(fSpeed);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return;
}

int KRLCharacter::UpdatePlayerWeaponSocket(BOOL bSheath)
{
    int nRetCode = false;
    int nResult = false;
    RL_SHEATH nSheathSocket = RL_SHEATH_NONE;
    LPCTSTR szSheathSocketNameLH = NULL;
    LPCTSTR szSheathSocketNameRH = NULL;
    LPCTSTR szWeaponSocketNameLH = NULL;
    LPCTSTR szWeaponSocketNameRH = NULL;

    KG_PROCESS_SUCCESS(m_EquipResource.nObject != RL_OBJECT_PLAYER);

    nSheathSocket = m_EquipResource.Player.nSheathSocket;

    szSheathSocketNameLH = GetPlayerSheathSocketLH(nSheathSocket);
    KGLOG_PROCESS_ERROR(szSheathSocketNameLH);

    szSheathSocketNameRH = GetPlayerSheathSocketRH(nSheathSocket);
    KGLOG_PROCESS_ERROR(szSheathSocketNameRH);

    if (bSheath)
    {
        szWeaponSocketNameLH = szSheathSocketNameLH;
        szWeaponSocketNameRH = szSheathSocketNameRH;
    }
    else
    {
        szWeaponSocketNameLH = _T("S_LH");
        szWeaponSocketNameRH = _T("S_RH");
    }

    m_bSheath = bSheath;

    nRetCode = BindSocket(MODEL_PLAYER_SOCKET_LH_SHEATH - MODEL_PLAYER_SOCKET_BEGIN, szSheathSocketNameLH);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BindSocket(MODEL_PLAYER_SOCKET_RH_SHEATH - MODEL_PLAYER_SOCKET_BEGIN, szSheathSocketNameRH);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BindSocket(MODEL_PLAYER_SOCKET_WEAPON_LH - MODEL_PLAYER_SOCKET_BEGIN, szWeaponSocketNameLH);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BindSocket(MODEL_PLAYER_SOCKET_WEAPON_RH - MODEL_PLAYER_SOCKET_BEGIN, szWeaponSocketNameRH);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpdateModel(KRLCharacterEquipResource const& EquipResource, BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

    if (m_pRLHorse)
    {
        nRetCode = UnloadRides(&m_pRLHorse);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_pRLVehicle)
    {
        nRetCode = UnloadRides(&m_pRLVehicle);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (m_RenderData.HasLoadedMDL())
    {
        hr = m_RenderData.Reset();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = LoadModel(EquipResource, bForceUpdate);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (m_FrameData.m_Current.bOnRide)
    {
        nRetCode = UpdateRide();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        hr = m_RenderData.Reset();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    return nResult;
}

int KRLCharacter::ResetSelectable()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = SetSelectable(m_EquipResource.MDL.bSelectable);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::SetSelectable(BOOL bSelectable)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    hr = m_RenderData.SetMDLSelectable(bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (m_pRLHorse)
    {
        hr = m_pRLHorse->m_RenderData.SetMDLSelectable(bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_pRLVehicle)
    {
        hr = m_pRLVehicle->m_RenderData.SetMDLSelectable(bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCharacter::LoadModel(KRLCharacterEquipResource const& EquipResource, BOOL bForceUpdate)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nEquipCount = 0;

    memcpy(&m_EquipResource, &EquipResource, sizeof(m_EquipResource));

    if (bForceUpdate)
    {
        hr = m_RenderData.SetMDLSelectable(EquipResource.MDL.bSelectable);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = m_RenderData.SetMDLVisible(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.LoadMDL(EquipResource.MDL.szMDLFilePath, EquipResource.MDL.fMDLScale, 
        EquipResource.nColorChannelTable, EquipResource.nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    KG_PROCESS_ERROR(m_RenderData.HasLoadedMDL());

    switch (EquipResource.nObject)
    {
    case RL_OBJECT_UNKNOWN:
        break;
    case RL_OBJECT_PLAYER:
        nEquipCount = _countof(EquipResource.Player.Models);
        break;
    case RL_OBJECT_NPC:
    case RL_OBJECT_DOODAD:
    case RL_OBJECT_RIDES:
        break;
    default:
        ASSERT(0);
        break;
    }

    for (int nIndex = 0; nIndex < nEquipCount; ++nIndex)
    {
        KRLCharacterModelResource const& part = EquipResource.Player.Models[nIndex];

        if (part.szMeshFilePath[0] != _T('\0'))
        {
            if (m_RenderData.HasLoadedModel(nIndex))
            {
                hr = m_RenderData.UnloadModel(nIndex);
                KGLOG_COM_CHECK_ERROR(hr);
            }

            hr = m_RenderData.LoadModel(nIndex, part.szMeshFilePath, part.szMtlFilePath, part.fMeshScale, 
                EquipResource.nColorChannelTable, part.nColorChannel);
            KGLOG_COM_PROCESS_ERROR(hr);

            if (part.szSFX1FilePath[0] != _T('\0'))
            {   
                hr = m_RenderData.LoadEquipSFX(nIndex * 2, part.szSFX1FilePath, part.fMeshScale);
                KGLOG_COM_CHECK_ERROR(hr);
            }

            if (part.szSFX2FilePath[0] != _T('\0'))
            {   
                hr = m_RenderData.LoadEquipSFX(nIndex * 2 + 1, part.szSFX2FilePath, part.fMeshScale);
                KGLOG_COM_CHECK_ERROR(hr);
            }
        }
    }

    for (int nIndex = 0; nIndex != CHARACTER_SOCKET_COUNT; ++nIndex)
    {
        hr = LoadSocket(nIndex, EquipResource.SocketParams[nIndex], EquipResource.nColorChannelTable);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    nRetCode = InitRLControl();
    KGLOG_PROCESS_ERROR(nRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLCharacter::LoadSocketByAniamtion(DWORD dwAnimationID)
{
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    int nResult = false;
    BOOL bLoadDefult = FALSE;
    KRLSocketResource AnimationSocketResource[CHARACTER_SOCKET_COUNT];

    KG_PROCESS_ERROR(m_RenderData.HasLoadedMDL());

    ASSERT(m_EquipResource.nObject == RL_OBJECT_NPC);

    nRetCode = GetNpcSocketByAnimation(m_EquipResource, dwAnimationID, AnimationSocketResource, bLoadDefult);
    KG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex != CHARACTER_SOCKET_COUNT; ++nIndex)
    {
        if (bLoadDefult && AnimationSocketResource[nIndex].szMeshFilePath[0] == _T('\0'))
        {
            hr = LoadSocket(nIndex, m_EquipResource.SocketParams[nIndex], m_EquipResource.nColorChannelTable);
            KGLOG_COM_CHECK_ERROR(hr);
        }
        else
        {
            hr = LoadSocket(nIndex, AnimationSocketResource[nIndex], m_EquipResource.nColorChannelTable);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLCharacter::LoadSocket(int nIndex, KRLSocketResource const SocketParam, int nColorChannelTable)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (SocketParam.szMeshFilePath[0] != _T('\0'))
    {   
        hr = m_RenderData.LoadSocket(nIndex, SocketParam.szSocketName, SocketParam.szMeshFilePath, SocketParam.szMtlFilePath, SocketParam.fMeshScale,
            nColorChannelTable, SocketParam.nColorChannel);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (SocketParam.szSFX1FilePath[0] != _T('\0'))
        {   
            hr = m_RenderData.LoadSocketSFX(nIndex * 2, SocketParam.szSFX1FilePath, SocketParam.fMeshScale);
            KGLOG_COM_PROCESS_ERROR(hr);
        }

        if (SocketParam.szSFX2FilePath[0] != _T('\0'))
        {   
            hr = m_RenderData.LoadSocketSFX(nIndex * 2 + 1, SocketParam.szSFX2FilePath, SocketParam.fMeshScale);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }
    else
    {
        if (m_RenderData.HasLoadedSocket(nIndex))
        {
            hr = m_RenderData.UnloadSocket(nIndex);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLCharacter::UpdateRide()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    KRidesModel const* pModel = NULL;
    DWORD dwObjectID = 0;
    DWORD dwRepresentID = 0;
    KRLRides* pRLRides = NULL;
    int nLoadRides = false;

    ASSERT(m_pRLHorse == NULL);

    KG_PROCESS_SUCCESS(m_EquipResource.nLoadOptimization == RL_MODEL_OPTIMIZATION_LOAD_NONE);

    dwObjectID = GetObjectID();
    dwRepresentID = m_EquipResource.Player.dwRepresentIDs[perHorseStyle];

    nRetCode = LoadRides(dwRepresentID, &pRLRides);
    KGLOG_PROCESS_ERROR(nRetCode);

    ASSERT(pRLRides);

    nLoadRides = true;

    // 换装需要重置马信息和重新绑马
    pModel = g_pRL->m_TableList.GetRidesModel(dwRepresentID);
    KGLOG_PROCESS_ERROR(pModel);

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    hr = m_RenderData.SetMDLVisible(FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLRides->AttachObject(dwObjectID);
    pRLRides->SetObjectType(RL_OBJECT_RIDES);

    pRLRides->m_pRLCharacter = this;
    pRLRides->m_dwRepresentID = dwRepresentID;
    pRLRides->m_dwIdleAnimationID = pModel->dwIdleAnimationID;
    pRLRides->m_dwSlopeType = pModel->nSlopeType;

    nRetCode = pRLRides->Init(pRLScene, m_bCurrentPlayer);
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = pRLRides->DisplayDataChanged();
    KGLOG_COM_PROCESS_ERROR(hr);

    pRLRides->SetAlpha(1.0f); 
    pRLRides->FadeIn();

    m_fRideAdjustHeight = pModel->fCameraAdjust;

    m_pRLHorse = pRLRides;
    pRLRides = NULL;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nLoadRides)
        {
            nRetCode = UnloadRides(&pRLRides);
            KGLOG_CHECK_ERROR(nRetCode);
        }
    }
    return nResult;
}

int KRLCharacter::BindSocket(int nSocket, LPCTSTR szSocket)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    if (m_RenderData.HasLoadedSocket(nSocket))
    {
        hr = m_RenderData.UnbindSocket(nSocket);
        KGLOG_COM_PROCESS_ERROR(hr);

        hr = m_RenderData.BindSocket(nSocket, szSocket);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    nResult = true;
Exit0:
    return nResult;
}

// 输入nAnimationTable为RL_ANIMATION_TABLE_类,输出dwAnimationID为最终动作编号
DWORD KRLCharacter::GetAnimationID(RL_ANIMATION_TABLE nAnimationTable, BOOL bSheathFlag) const
{
    int nRetCode = false;
    DWORD dwAnimationID = 0;
    RL_WEAPON nWeaponType = RL_WEAPON_NONE;
    KRLAnimationIndex AnimationIndex;
    KRLAdjustAnimationParam Param;

    nRetCode = FillRLAdjustAnimationParam(&Param, bSheathFlag);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterAnimationIndex(m_EquipResource, 
        (RL_SCHOOL)m_FrameData.m_Current.nSchoolID, 
        nAnimationTable, Param.nWeaponType, &Param, &AnimationIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    switch (AnimationIndex.dwAnimationIndexType)
    {
    case RL_ANIMATION_INDEX_NONE:
        dwAnimationID = AnimationIndex.dwID;
        break;
    case RL_ANIMATION_INDEX_RANDOM:
        dwAnimationID = GetCharacterRandomAnimation(m_EquipResource, AnimationIndex.dwID);
        break;
    case RL_ANIMATION_INDEX_SEQUENCE:
        // TODO:
        dwAnimationID = AnimationIndex.dwID;
        break;
    }

Exit0:
    return dwAnimationID;
}

RL_ANIMATION_TABLE KRLCharacter::GetAnimationTable(RL_ANIMATION_DOWN nDownType, RL_ADVANCE nAdvance, RL_FIGHT_FLAG nFightFlag) const
{
    KRLAnimationTableParam Param;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;

    ASSERT(nDownType != RL_ANIMATION_DOWN_AUTOFLY);

    Param.nDownType = nDownType;
    Param.nAdvance = nAdvance;
    Param.nFightFlag = nFightFlag;
    Param.bSheathFlag = m_FrameData.m_Current.bSheathFlag;
    
    nAnimationTable = ::GetAnimationTable(Param);

    return nAnimationTable;
}

int KRLCharacter::UpdateAnimation(RL_ANIMATION_DOWN nAnimationDown, RL_ANIMATION_STATE nAnimationState, RL_ADVANCE nAdvance,
                                  RL_FIGHT_FLAG nFightFlag,
                                  DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT])
{
    int nRetCode = false;
    int nResult = false;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    DWORD dwAnimationID = 0;

    if (m_nDownBodyState != RL_ANIMATION_DOWN_NONE && m_nDownBodyState != nAnimationDown)
    {
        // 后置动作
        nAnimationTable = GetStateOutAnimationTable(m_nDownBodyState, nAnimationDown, m_AnimationStatus.nAdvance);

        if (nAnimationTable != RL_ANIMATION_TABLE_NONE)
        {
            dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

            nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_NONE, nAnimationDown);
            KGLOG_PROCESS_ERROR(nRetCode);

            goto Exit1;
        }

        // 前置动作
        nAnimationTable = GetStateInAnimationTable(nAnimationDown, m_nDownBodyState, nAdvance);

        if (nAnimationTable != RL_ANIMATION_TABLE_NONE)
        {
            dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

            nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_NONE, nAnimationDown);
            KGLOG_PROCESS_ERROR(nRetCode);

            goto Exit1;
        }
    }

    if (m_pDownBodyCurrentAnimationTrack->nState == RL_ANIMATION_STATE_IMPULSE || 
        m_nDownBodyState != nAnimationDown || 
        m_AnimationStatus.nAdvance != nAdvance ||
        m_nFightFlag != nFightFlag ||
        memcmp(m_dwSkillBuffs, dwSkillBuffs, sizeof(m_dwSkillBuffs)) != 0)
    {
        // 状态动作

        if (m_nDownBodyState == RL_ANIMATION_DOWN_AUTOFLY)
        {
            KGLOG_PROCESS_ERROR(m_pRLVehicle);

            dwAnimationID = m_pRLVehicle->m_dwIdleAnimationID;
        }
        else
        {
            nAnimationTable = GetAnimationTable(nAnimationDown, nAdvance, nFightFlag);
            dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);
        }

        nRetCode = PlayAnimation(dwAnimationID, nAnimationState, RL_ANIMATION_UP_UNKNOWN, nAnimationDown);
        KGLOG_PROCESS_ERROR(nRetCode);

        m_AnimationStatus.nAdvance = nAdvance;

        m_nFightFlag = nFightFlag;

        memcpy(m_dwSkillBuffs, dwSkillBuffs, sizeof(m_dwSkillBuffs));

        goto Exit1;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::TurnPlayerFaceBegin(float fDeltaDirection)
{
    static float const DIRECTION_MIN = -D3DX_PI / 2.0f;
    static float const DIRECTION_MAX = D3DX_PI / 2.0f;
    static float const HEAD_RATIO = 0.6f;
    static float const SPINE_RATIO = 0.4f;

    HRESULT hr = E_FAIL;
    BoneTransformationInputData Data;
    float fHeadDirection = 0.0f;
    float fSpineDirection = 0.0f;

    Data.Type = ROTATION_LOCALE_AXIS;

    fDeltaDirection = min(fDeltaDirection, DIRECTION_MAX);
    fDeltaDirection = max(fDeltaDirection, DIRECTION_MIN);

    fHeadDirection = fDeltaDirection * HEAD_RATIO;
    fSpineDirection = fDeltaDirection * SPINE_RATIO;

    Data.Data = &fSpineDirection;

    hr = m_RenderData.SetBoneTransform("Bip01 Spine1", Data);
    KGLOG_COM_PROCESS_ERROR(hr);

    Data.Data = &fHeadDirection;

    hr = m_RenderData.SetBoneTransform("Bip01 Head", Data);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.EnableBoneTransform(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

void KRLCharacter::TurnNpcFaceBegin(float fDeltaDirection)
{
    // DO Nothing !
}

void KRLCharacter::TurnPlayerFaceEnd()
{
    HRESULT hr = E_FAIL;

    hr = m_RenderData.EnableBoneTransform(FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

void KRLCharacter::TurnNpcFaceEnd()
{
    // Do Nothing !
}

int KRLCharacter::PlaySkillEffectResultAnimation(RL_ANIMATION_UP nAnimationUp, RL_ANIMATION_TABLE nAnimationTable, KSkillCasterModel const* pSkillCasterModel)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwAnimationID = 0;

	KG_ASSERT_EXIT(nAnimationTable != RL_ANIMATION_TABLE_NONE);

    KG_PROCESS_SUCCESS(pSkillCasterModel == NULL);
    KG_PROCESS_SUCCESS(!pSkillCasterModel->bPlayBehitAnimation);

    if (m_pDownBodyCurrentAnimationTrack->nAnimationDownType == RL_ANIMATION_DOWN_KNOCK_DOWN)
        nAnimationTable = GetKnowDownIdleBeHitAnimationTable(nAnimationTable);

    dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

    nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, nAnimationUp, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::PlaySkillEffectResultSFX(DWORD dwEffectIDs[CHARACTER_SFX_SKILL_EFFECT_COUNT], DWORD dwCasterID)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DXVECTOR3 vCaster(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vTarget(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vOffset(0.0f, 0.0f, 0.0f);
    KRLCharacter* pRLCharacter = NULL;

    pRLCharacter = GetRLCharacter(dwCasterID);
    KG_PROCESS_ERROR(pRLCharacter);

    for (int nIndex = CHARACTER_SFX_SKILL_EFFECT_BEGIN; nIndex != CHARACTER_SFX_SKILL_EFFECT_END; ++nIndex)
    {
        DWORD dwEffectResultID = dwEffectIDs[nIndex - CHARACTER_SFX_SKILL_EFFECT_BEGIN];

        KSkillResultModel const* pSkillResultModel = g_pRL->m_TableList.GetSkillResultModel(dwEffectResultID, m_EquipResource.dwEffectType);
        if (pSkillResultModel == NULL || !pSkillResultModel->dwEffectID)
            continue;

        KSkillEffectModel const* pSkillEffectModel = g_pRL->m_TableList.GetSkillEffectModel(pSkillResultModel->dwEffectID);
        if (pSkillEffectModel == NULL)
            continue;

        KRLSFXParam SFXParam;

        SFXParam.dwPlayType = ENU_ANIMATIONPLAY_ADDCURRENT_ONCE;
        SFXParam.fSpeed = 1.0f;
        SFXParam.fScale = pSkillEffectModel->fSFXScale * m_EquipResource.fEffectScale;
        SFXParam.szBoneName = pSkillEffectModel->szBoneName;
        SFXParam.szFilePath = pSkillEffectModel->szSFXFilePath;

        if (SFXParam.szFilePath[0] != _T('\0'))
        {
            IKG3DModel* p3DModel = NULL;

            // 骨骼不存在则绑在原点
            if (SFXParam.szBoneName[0] == _T('\0') || m_RenderData.GetBonePosition(vTarget, vOffset, SFXParam.szBoneName) != S_OK)
                GetPosition(vTarget);

            if (pSkillEffectModel->bBind)
            {
                m_RenderData.LoadMDLSFX(nIndex, SFXParam);

                if (pSkillEffectModel->bAdjustDirection)
                {
                    vOffset = vTarget - vCaster;

                    hr = m_RenderData.SetMDLSFXOffsetDirection(nIndex, vOffset);
                    KGLOG_COM_PROCESS_ERROR(hr);
                }
            }
            else
            {
                if (SFXParam.szFilePath[0] != _T('\0'))
                {
                    KRLScene* pRLScene = m_RenderData.GetRLScene();
                    if (pRLScene)
                    {
                        p3DModel = pRLScene->m_ModelGCMgr.CreateAutoRemoveModel(SFXParam.szFilePath, pRLScene);
                        if (p3DModel)
                        {
                            KRLMonoAnimationParam MonoAnimationParam;

                            hr = KModel::SetScale(p3DModel, SFXParam.fScale);
                            KGLOG_COM_CHECK_ERROR(hr);

                            hr = KModel::SetPosition(p3DModel, vTarget);
                            KGLOG_COM_CHECK_ERROR(hr);

                            MonoAnimationParam.dwPlayType = SFXParam.dwPlayType;
                            MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
                            MonoAnimationParam.fSpeed = SFXParam.fSpeed;
                            MonoAnimationParam.nOffsetTime = 0;
                            MonoAnimationParam.dwTweenTime = 0;
                            MonoAnimationParam.pUserdata = NULL;
                            MonoAnimationParam.szFilePath = _T("");
                            MonoAnimationParam.fStartTime = g_pRL->m_fTime;

                            hr = KModel::PlayMonoAnimation(p3DModel, MonoAnimationParam);
                            KGLOG_COM_PROCESS_ERROR(hr);

                            if (pSkillEffectModel->bAdjustDirection)
                            {
                                vOffset = vTarget - vCaster;

                                hr = KModel::SetOffsetDirection(p3DModel, vOffset);
                                KGLOG_COM_PROCESS_ERROR(hr);
                            }
                        }
                    }
                }
            }
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::CanPlaySplitAnimation(enuAnimationJointType nJointType, DWORD dwAnimationID, DWORD dwAnimationTrackID, KRLAnimationParam const& Animation) const
{
    int nRetCode = false;
    int nResult = false;
    int nAnimationRelation = 0;
    int nHitAnimationPlayRate = false;
    DWORD dwPrevAnimationKindID = 0;
    DWORD dwPrevAnimationID = 0;
    DWORD dwPrevAnimationTrackID = 0;
    RL_ANIMATION_TRACK nAnimationTrack = RL_ANIMATION_TRACK_DOWN;

    ASSERT(m_RenderData.HasLoadedMDL());

    KG_PROCESS_ERROR(dwAnimationID != 0);

    if (nJointType == ANI_JOINT_TOP)
    {
        nAnimationTrack = RL_ANIMATION_TRACK_UP;
        dwPrevAnimationTrackID = m_UpBodyHighAnimationTrack.dwTrackID;
    }
    else
    {
        nAnimationTrack = RL_ANIMATION_TRACK_DOWN;
        dwPrevAnimationTrackID = m_DownBodyHighAnimationTrack.dwTrackID;
    }

    // 轨道优先级
    KG_PROCESS_ERROR(dwAnimationTrackID == 0 || dwAnimationTrackID >= dwPrevAnimationTrackID);

    // 动作优先级
    if (dwAnimationTrackID > 0)
    {
        // high track

        if (nJointType == ANI_JOINT_TOP)
        {
            dwPrevAnimationKindID = m_UpBodyHighAnimationTrack.dwKindID;
            dwPrevAnimationID = m_UpBodyHighAnimationTrack.dwAnimationID;
        }
        else
        {
            dwPrevAnimationKindID = m_DownBodyHighAnimationTrack.dwKindID;
            dwPrevAnimationID = m_DownBodyHighAnimationTrack.dwAnimationID;
        }
    }
    else
    {
        // low track

        if (nJointType == ANI_JOINT_TOP)
        {
            dwPrevAnimationKindID = m_UpBodyLowAnimationTrack.dwKindID;
            dwPrevAnimationID = m_UpBodyLowAnimationTrack.dwAnimationID;
        }
        else
        {
            dwPrevAnimationKindID  = m_DownBodyLowAnimationTrack.dwKindID;
            dwPrevAnimationID = m_DownBodyLowAnimationTrack.dwAnimationID;
        }
    }

    KG_PROCESS_ERROR(dwPrevAnimationID != dwAnimationID);

    nHitAnimationPlayRate = g_pRL->m_TableList.HitAnimationPlayRate(Animation.fRatio); 
    KG_PROCESS_ERROR(nHitAnimationPlayRate);

    nAnimationRelation = g_pRL->m_TableList.GetCharacterAnimationRelation(m_EquipResource.nObject == RL_OBJECT_PLAYER, 
        dwPrevAnimationKindID, Animation.dwKindID, nAnimationTrack);
    KG_PROCESS_ERROR(nAnimationRelation > 0);

    nResult = true;
Exit0:
    return nResult;
}

// 动作轨道优先级(A)和播放优先级(B)的运作原则:
// 1、记录信息为当前客户端表现的动作信息
// 2、先比较A,后与对应轨道的动作比较B
// 3.1、请求动作的A为非0则与高轨动作的A比较,大于等于则比较两者的B
// 3.2、请求动作的A为0则与低轨动作比较两者的B
int KRLCharacter::PlaySplitAnimation(enuAnimationJointType nJointType, DWORD dwAnimationID, RL_ANIMATION_STATE nState, 
                                      int nAnimationState, int nOffsetTime, void* pUserdata)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLAnimationParam Animation;
    KRLSplitAnimationParam* pSplitAnimationParam = NULL;
    DWORD dwPrevAnimationID = 0;
    DWORD dwAnimationTrackID = 0;
    enuAnimationControllerPriority nTrackType = ANICTL_PRIMARY;

    if (nJointType == ANI_JOINT_TOP)
    {
        if (nAnimationState != RL_ANIMATION_UP_UNKNOWN)
        {
            ASSERT(nAnimationState >= 0 && nAnimationState < RL_ANIMATION_UP_COUNT);
            m_nUpBodyState = (RL_ANIMATION_UP)nAnimationState;
        }
    }
    else
    {
        if (nAnimationState != RL_ANIMATION_DOWN_UNKNOWN)
        {
            ASSERT(nAnimationState >= 0 && nAnimationState < RL_ANIMATION_DOWN_COUNT);
            m_nDownBodyState = (RL_ANIMATION_DOWN)nAnimationState;
        }
    }

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    ASSERT(m_AnimationStatus.nType == RL_ANIMATION_SPLIT);

#ifdef RL_TRACE_OPTION
    if (g_pRL->m_CurrentTraceOption.bTracePlayerAnimation)
    {
        LPCTSTR szAnimationStateName = NULL;

        if (nJointType == ANI_JOINT_TOP)
            szAnimationStateName = GetAnimationUpName((RL_ANIMATION_UP)nAnimationState);
        else
            szAnimationStateName = GetAnimationDownName((RL_ANIMATION_DOWN)nAnimationState);

        cprintf("Try PlaySplitAnimation(jointtype:%d, animationid:%d, state:%s, up/down state:%s)\n", 
            nJointType, 
            dwAnimationID, 
            nState == RL_ANIMATION_STATE_IMPULSE ? "impluse" : "hold", 
            szAnimationStateName);
    }
#endif  // RL_TRACE_OPTION

    // 轨道优先级
    nRetCode = GetCharacterAnimation(m_EquipResource, dwAnimationID, &Animation);
    KG_PROCESS_ERROR(nRetCode);

    dwAnimationTrackID = GetAnimationTrackID(nJointType, Animation.dwKindID);

    if (dwAnimationTrackID > 0)
        nTrackType = ANICTL_SECONDARY;

    nRetCode = !CanPlaySplitAnimation(nJointType, dwAnimationID, dwAnimationTrackID, Animation);
    KG_PROCESS_SUCCESS(nRetCode);

    // 播放动作

    if (nJointType == ANI_JOINT_TOP)
    {
        if (nTrackType == ANICTL_SECONDARY)
            pSplitAnimationParam = &m_AnimationStatus.SplitStatus.UpHighTrackParam;
        else
            pSplitAnimationParam = &m_AnimationStatus.SplitStatus.UpLowTrackParam;

        dwPrevAnimationID = m_pUpBodyCurrentAnimationTrack->dwAnimationID;
    }
    else
    {
        if (nTrackType == ANICTL_SECONDARY)
            pSplitAnimationParam = &m_AnimationStatus.SplitStatus.DownHighTrackParam;
        else
            pSplitAnimationParam = &m_AnimationStatus.SplitStatus.DownLowTrackParam;

        dwPrevAnimationID = m_pDownBodyCurrentAnimationTrack->dwAnimationID;
    }

    nRetCode = GetCharacterAnimationFusion(m_EquipResource, dwAnimationID, dwPrevAnimationID, &Animation.SplitAnimationParam.dwTweenTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    memcpy(pSplitAnimationParam, &Animation.SplitAnimationParam, sizeof(KRLSplitAnimationParam));

    pSplitAnimationParam->nTrackType = nTrackType;
    pSplitAnimationParam->pUserdata = pUserdata;
    pSplitAnimationParam->dwSplitType = Animation.dwSplitType;
    pSplitAnimationParam->dwExtraInfo = nJointType;
    pSplitAnimationParam->fStartTime = g_pRL->m_fTime;

    if (nState == RL_ANIMATION_STATE_HOLD_ONCE && pSplitAnimationParam->dwPlayType == ENU_ANIMATIONPLAY_ADDCURRENT_ONCE)
        pSplitAnimationParam->nOffsetTime = ANIMATION_SEEK_OFFSET_END;
    else
        pSplitAnimationParam->nOffsetTime = nOffsetTime;

    hr = m_RenderData.PlaySplitAnimation(*pSplitAnimationParam);
    KGLOG_COM_PROCESS_ERROR(hr);

    // 设置参数
    if (nJointType == ANI_JOINT_TOP)
    {
        if (dwAnimationTrackID > 0)
        {
            ASSERT(nState == RL_ANIMATION_STATE_IMPULSE);

            m_UpBodyHighAnimationTrack.nAnimationUpType = m_nUpBodyState;
            m_UpBodyHighAnimationTrack.nSheathSocket = Animation.nSheathSocket;
            m_UpBodyHighAnimationTrack.dwAnimationID = dwAnimationID;
            m_UpBodyHighAnimationTrack.dwKindID = Animation.dwKindID;
            m_UpBodyHighAnimationTrack.dwTrackID = dwAnimationTrackID;
            m_UpBodyHighAnimationTrack.nState = nState;

            m_pUpBodyCurrentAnimationTrack = &m_UpBodyHighAnimationTrack;

            UpdateWeaponPosition(Animation.nSheathSocket, m_FrameData.m_Current.bSheathFlag);
        }
        else
        {
            m_UpBodyLowAnimationTrack.nAnimationUpType = m_nUpBodyState;
            m_UpBodyLowAnimationTrack.nSheathSocket = Animation.nSheathSocket;
            m_UpBodyLowAnimationTrack.dwAnimationID = dwAnimationID;
            m_UpBodyLowAnimationTrack.dwKindID = Animation.dwKindID;
            m_UpBodyLowAnimationTrack.dwTrackID = dwAnimationTrackID;
            m_UpBodyLowAnimationTrack.nState = nState;

            if (m_pUpBodyCurrentAnimationTrack == &m_UpBodyLowAnimationTrack)
            {
                UpdateWeaponPosition(Animation.nSheathSocket, m_FrameData.m_Current.bSheathFlag);
            }
        }
    }
    else
    {
        if (dwAnimationTrackID > 0)
        {
            ASSERT(nState == RL_ANIMATION_STATE_IMPULSE);

            m_DownBodyHighAnimationTrack.nAnimationDownType = m_nDownBodyState;
            m_DownBodyHighAnimationTrack.dwAnimationID = dwAnimationID;
            m_DownBodyHighAnimationTrack.dwKindID = Animation.dwKindID;
            m_DownBodyHighAnimationTrack.dwTrackID = dwAnimationTrackID;
            m_DownBodyHighAnimationTrack.nState = nState;

            m_pDownBodyCurrentAnimationTrack = &m_DownBodyHighAnimationTrack;
        }
        else
        {
            m_DownBodyLowAnimationTrack.nAnimationDownType = m_nDownBodyState;
            m_DownBodyLowAnimationTrack.dwAnimationID = dwAnimationID;
            m_DownBodyLowAnimationTrack.dwKindID = Animation.dwKindID;
            m_DownBodyLowAnimationTrack.dwTrackID = dwAnimationTrackID;
            m_DownBodyLowAnimationTrack.nState = nState;
        }

        if (m_pRLHorse)
        {
            hr = m_pRLHorse->PlayRideAnimation(dwAnimationID, nOffsetTime, nTrackType);
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (m_pRLVehicle)
        {
            hr = m_pRLVehicle->PlayRideAnimation(dwAnimationID, nOffsetTime, nTrackType);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

void KRLCharacter::PlayWholeAnimation(KRLAniInfo Info)
{
    KRLAnimationParam Animation;
    HRESULT hr = E_FAIL;
    int nRetCode = false;
    DWORD dwTrackID = 0;
    DWORD dwOldKind = 0;
    DWORD dwPrevAnimationID = 0;

    if (Info.nUpState != RL_ANIMATION_UP_UNKNOWN)
    {
        ASSERT(Info.nUpState >= 0 && Info.nUpState < RL_ANIMATION_UP_COUNT);
        m_nUpBodyState = (RL_ANIMATION_UP)Info.nUpState;
    }

    if (Info.nDownState != RL_ANIMATION_DOWN_UNKNOWN)
    {
        ASSERT(Info.nDownState >= 0 && Info.nDownState < RL_ANIMATION_DOWN_COUNT);
        m_nDownBodyState = (RL_ANIMATION_DOWN)Info.nDownState;
    }

    KG_PROCESS_SUCCESS(!Info.dwAnimationID);
    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    // 轨道优先级
    nRetCode = GetCharacterAnimation(m_EquipResource, Info.dwAnimationID, &Animation);
    KG_PROCESS_ERROR(nRetCode);

    dwTrackID = g_pRL->m_TableList.GetNPCAnimationTrackType(Animation.dwKindID);

    KG_PROCESS_SUCCESS(dwTrackID && dwTrackID < m_DownBodyHighAnimationTrack.dwTrackID);

    // 动作优先级
    if (dwTrackID != 0)
    {
        dwOldKind  = m_DownBodyHighAnimationTrack.dwKindID;
        dwPrevAnimationID = m_DownBodyHighAnimationTrack.dwAnimationID;
    }
    else
    {
        dwOldKind  = m_DownBodyLowAnimationTrack.dwKindID;
        dwPrevAnimationID = m_DownBodyLowAnimationTrack.dwAnimationID;
    }

    ASSERT(m_EquipResource.nObject == RL_OBJECT_NPC);

    KG_PROCESS_SUCCESS(!g_pRL->m_TableList.GetCharacterAnimationRelation(FALSE, dwOldKind, Animation.dwKindID, RL_ANIMATION_TRACK_DOWN));

    if ((dwTrackID || m_pDownBodyCurrentAnimationTrack == &m_DownBodyLowAnimationTrack) && Info.dwAnimationID != dwPrevAnimationID)
        UpdateNpcModel(Info.dwAnimationID);

    // 播放动作
    ASSERT(m_AnimationStatus.nType == RL_ANIMATION_MONO);

    nRetCode = GetCharacterAnimationFusion(m_EquipResource, Info.dwAnimationID, dwPrevAnimationID, &Animation.MonoAnimationParam.dwTweenTime);
    KG_PROCESS_ERROR(nRetCode);

    memcpy(&m_AnimationStatus.MonoStatus.MonoParam, &Animation.MonoAnimationParam, sizeof(KRLMonoAnimationParam));

    m_AnimationStatus.MonoStatus.MonoParam.nTrackType = dwTrackID ? ANICTL_SECONDARY : ANICTL_PRIMARY;
    m_AnimationStatus.MonoStatus.MonoParam.pUserdata = Info.pUserdata;
    m_AnimationStatus.MonoStatus.MonoParam.fStartTime = g_pRL->m_fTime;

    if (m_nDownBodyState == RL_ANIMATION_DOWN_DEATH && Info.nState != RL_ANIMATION_STATE_HOLD_LOOP)
    {
        // FIXED: Ugly patch, promote animation priority of death

        Info.nState = RL_ANIMATION_STATE_HOLD_ONCE;

        ASSERT(m_AnimationStatus.MonoStatus.MonoParam.dwPlayType == ENU_ANIMATIONPLAY_ADDCURRENT_ONCE);

        m_AnimationStatus.MonoStatus.MonoParam.nTrackType = ANICTL_PRIMARY;
        m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime = ANIMATION_SEEK_OFFSET_END;

        hr = m_RenderData.PlayMonoAnimation(m_AnimationStatus.MonoStatus.MonoParam);
        KG_COM_PROCESS_ERROR(hr);

        m_AnimationStatus.MonoStatus.MonoParam.nTrackType = ANICTL_SECONDARY;
        m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime = Info.nOffsetTime;

        hr = m_RenderData.PlayMonoAnimation(m_AnimationStatus.MonoStatus.MonoParam);
        KG_COM_PROCESS_ERROR(hr);

        m_pUpBodyCurrentAnimationTrack = &m_UpBodyLowAnimationTrack;
        m_pDownBodyCurrentAnimationTrack = &m_DownBodyLowAnimationTrack;
    }
    else
    {
        if (Info.nState == RL_ANIMATION_STATE_HOLD_ONCE && m_AnimationStatus.MonoStatus.MonoParam.dwPlayType == ENU_ANIMATIONPLAY_ADDCURRENT_ONCE)
            m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime = ANIMATION_SEEK_OFFSET_END;
        else
            m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime = Info.nOffsetTime;

        hr = m_RenderData.PlayMonoAnimation(m_AnimationStatus.MonoStatus.MonoParam);
        KG_COM_PROCESS_ERROR(hr);
    }

    // 设置参数

    if (dwTrackID)
    {
        m_UpBodyHighAnimationTrack.nAnimationUpType = m_nUpBodyState;
        m_UpBodyHighAnimationTrack.dwAnimationID = Info.dwAnimationID;
        m_UpBodyHighAnimationTrack.dwKindID = Animation.dwKindID;
        m_UpBodyHighAnimationTrack.dwTrackID = dwTrackID;
        m_UpBodyHighAnimationTrack.nState = Info.nState;

        m_DownBodyHighAnimationTrack.nState = m_UpBodyHighAnimationTrack.nState;
        m_DownBodyHighAnimationTrack.dwAnimationID = m_UpBodyHighAnimationTrack.dwAnimationID;
        m_DownBodyHighAnimationTrack.dwKindID = m_UpBodyHighAnimationTrack.dwKindID;
        m_DownBodyHighAnimationTrack.dwTrackID = m_UpBodyHighAnimationTrack.dwTrackID;

        m_DownBodyHighAnimationTrack.nAnimationDownType = m_nDownBodyState;

        m_pUpBodyCurrentAnimationTrack = &m_UpBodyHighAnimationTrack;
        m_pDownBodyCurrentAnimationTrack = &m_DownBodyHighAnimationTrack;
    }
    else
    {
        m_UpBodyLowAnimationTrack.nAnimationUpType = m_nUpBodyState;
        m_UpBodyLowAnimationTrack.dwAnimationID = Info.dwAnimationID;
        m_UpBodyLowAnimationTrack.dwKindID = Animation.dwKindID;
        m_UpBodyLowAnimationTrack.dwTrackID = dwTrackID;
        m_UpBodyLowAnimationTrack.nState = Info.nState;

        m_DownBodyLowAnimationTrack.nState = m_UpBodyLowAnimationTrack.nState;
        m_DownBodyLowAnimationTrack.dwAnimationID = m_UpBodyLowAnimationTrack.dwAnimationID;
        m_DownBodyLowAnimationTrack.dwKindID = m_UpBodyLowAnimationTrack.dwKindID;
        m_DownBodyLowAnimationTrack.dwTrackID = m_UpBodyLowAnimationTrack.dwTrackID;

        m_DownBodyLowAnimationTrack.nAnimationDownType = m_nDownBodyState;

        if (m_pDownBodyCurrentAnimationTrack == &m_DownBodyLowAnimationTrack)
        {
            m_pUpBodyCurrentAnimationTrack = &m_UpBodyLowAnimationTrack;
        }
    }

Exit0:
Exit1:
    return;
}

void KRLCharacter::PlayPlayerAnimation(KRLAniInfo const& AniInfo)
{
    PlaySplitAnimation(ANI_JOINT_BOTTOM, AniInfo.dwAnimationID, AniInfo.nState, AniInfo.nDownState, AniInfo.nOffsetTime, AniInfo.pUserdata);
    PlaySplitAnimation(ANI_JOINT_TOP, AniInfo.dwAnimationID, AniInfo.nState, AniInfo.nUpState, AniInfo.nOffsetTime, AniInfo.pUserdata);
}

void KRLCharacter::PlayNpcAnimation(KRLAniInfo const& AniInfo)
{
    PlayWholeAnimation(AniInfo);
}

int KRLCharacter::PauseAnimation(BOOL bPause)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    m_bAnimationPaused = bPause;

    hr = m_RenderData.PauseAnimation(bPause);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::PauseAnimationStatus(KRLCharacterDisplayData* pRLCharacterDisplayData)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(pRLCharacterDisplayData);

    pRLCharacterDisplayData->bSplitModel = m_EquipResource.bSplitModel;
    pRLCharacterDisplayData->dwModelID = m_EquipResource.dwModelID;
    pRLCharacterDisplayData->pRLHorse = m_pRLHorse;
    pRLCharacterDisplayData->pRLVehicle = m_pRLVehicle;

    switch (m_AnimationStatus.nType)
    {
    case RL_ANIMATION_NONE:
        break;
    case RL_ANIMATION_MONO:
        m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.MonoStatus.MonoParam.fStartTime);
        m_AnimationStatus.MonoStatus.MonoParam.dwTweenTime = 0;

        nRetCode = WholeBodyHighTrackAnimationFinish(); // patch
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_SPLIT:
        // down
        m_AnimationStatus.SplitStatus.DownLowTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.DownLowTrackParam.fStartTime);
        m_AnimationStatus.SplitStatus.DownLowTrackParam.dwTweenTime = 0;

        m_AnimationStatus.SplitStatus.DownHighTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.DownHighTrackParam.fStartTime);
        m_AnimationStatus.SplitStatus.DownHighTrackParam.dwTweenTime = 0;

        nRetCode = DownBodyHighTrackAnimationFinish(); // patch
        KGLOG_PROCESS_ERROR(nRetCode);

        // up
        m_AnimationStatus.SplitStatus.UpLowTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.UpLowTrackParam.fStartTime);
        m_AnimationStatus.SplitStatus.UpLowTrackParam.dwTweenTime = 0;

        m_AnimationStatus.SplitStatus.UpHighTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.UpHighTrackParam.fStartTime);
        m_AnimationStatus.SplitStatus.UpHighTrackParam.dwTweenTime = 0;

        nRetCode = UpBodyHighTrackAnimationFinish(); // patch
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    default:
        ASSERT(0);
        break;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(&m_AnimationStatus, 0, sizeof(KRLAnimationStatus));

        if (pRLCharacterDisplayData)
        {
            memset(pRLCharacterDisplayData, 0, sizeof(KRLCharacterDisplayData));
        }
    }
    return nResult;
}

int KRLCharacter::ResumeAnimationStatus(KRLCharacterDisplayData const* pRLCharacterDisplayData)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    RL_ANIMATION_STATE nAnimationState = RL_ANIMATION_STATE_HOLD_ONCE;
    DWORD dwAnimationID = 0;

    KGLOG_PROCESS_ERROR(pRLCharacterDisplayData);

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    if ((pRLCharacterDisplayData->dwModelID == m_EquipResource.dwModelID) && 
        (pRLCharacterDisplayData->bSplitModel == m_EquipResource.bSplitModel) && 
        (pRLCharacterDisplayData->pRLVehicle == m_pRLVehicle) &&
        (pRLCharacterDisplayData->pRLHorse == NULL && m_pRLHorse) &&
        (m_dwSerialAnimationID == 0))
    {
        // 相同模型

        switch (m_AnimationStatus.nType)
        {
        case RL_ANIMATION_NONE:
            break;
        case RL_ANIMATION_MONO:
            if (m_AnimationStatus.MonoStatus.MonoParam.szFilePath)
            {
                hr = m_RenderData.PlayMonoAnimation(m_AnimationStatus.MonoStatus.MonoParam);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
            break;
        case RL_ANIMATION_SPLIT:
            if (m_AnimationStatus.SplitStatus.DownLowTrackParam.szFilePath)
            {
                ASSERT(m_AnimationStatus.SplitStatus.DownLowTrackParam.nTrackType == ANICTL_PRIMARY);

                hr = m_RenderData.PlaySplitAnimation(m_AnimationStatus.SplitStatus.DownLowTrackParam);
                KGLOG_COM_PROCESS_ERROR(hr);
            }

            if (m_AnimationStatus.SplitStatus.DownHighTrackParam.szFilePath)
            {
                ASSERT(m_AnimationStatus.SplitStatus.DownHighTrackParam.nTrackType == ANICTL_SECONDARY);

                hr = m_RenderData.PlaySplitAnimation(m_AnimationStatus.SplitStatus.DownHighTrackParam);
                KGLOG_COM_PROCESS_ERROR(hr);
            }

            if (m_AnimationStatus.SplitStatus.UpLowTrackParam.szFilePath)
            {
                ASSERT(m_AnimationStatus.SplitStatus.UpLowTrackParam.nTrackType == ANICTL_PRIMARY);

                hr = m_RenderData.PlaySplitAnimation(m_AnimationStatus.SplitStatus.UpLowTrackParam);
                KGLOG_COM_PROCESS_ERROR(hr);
            }

            if (m_AnimationStatus.SplitStatus.UpHighTrackParam.szFilePath)
            {
                ASSERT(m_AnimationStatus.SplitStatus.UpHighTrackParam.nTrackType == ANICTL_SECONDARY);

                hr = m_RenderData.PlaySplitAnimation(m_AnimationStatus.SplitStatus.UpHighTrackParam);
                KGLOG_COM_PROCESS_ERROR(hr);
            }
            break;
        default:
            ASSERT(0);
            break;
        }
    }
    else
    {
        // 换过模型

        if (m_nDownBodyState == RL_ANIMATION_DOWN_NONE)
        {
            m_nDownBodyState = GetRLAnimationDown(
                m_FrameData.m_Current.nMoveState, 
                m_FrameData.m_Current.nOTAction,
                m_FrameData.m_Current.nJumpCount,
                RL_TURNING_STAND_NONE);
        }

        if (m_nDownBodyState == RL_ANIMATION_DOWN_AUTOFLY)
        {
            KGLOG_PROCESS_ERROR(m_pRLVehicle);

            dwAnimationID = m_pRLVehicle->m_dwIdleAnimationID;
        }
        else
        {
            nAnimationTable = GetAnimationTable(m_nDownBodyState, m_AnimationStatus.nAdvance, m_nFightFlag);

            dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);
        }

        if (m_nDownBodyState == RL_ANIMATION_DOWN_TURN_LEFT || m_nDownBodyState == RL_ANIMATION_DOWN_TURN_RIGHT)
        {
            if (m_FrameData.m_Current.nMoveState == cmsOnSwim || m_FrameData.m_Current.nMoveState == cmsOnFloat)
            {
                m_nDownBodyState = RL_ANIMATION_DOWN_FLOAT;
            }
            else
            {
                m_nDownBodyState = RL_ANIMATION_DOWN_STAND;
            }
        }

        if (!m_EquipResource.bSplitModel && m_nDownBodyState == RL_ANIMATION_DOWN_STAND)
        {
            // npc idle 的时候循环播放
            nAnimationState = RL_ANIMATION_STATE_HOLD_LOOP;
        }

        memset(&m_UpBodyHighAnimationTrack, 0, sizeof(m_UpBodyHighAnimationTrack));
        memset(&m_DownBodyHighAnimationTrack, 0, sizeof(m_DownBodyHighAnimationTrack));

        memset(&m_UpBodyLowAnimationTrack, 0, sizeof(m_UpBodyLowAnimationTrack));
        memset(&m_DownBodyLowAnimationTrack, 0, sizeof(m_DownBodyLowAnimationTrack));

        nRetCode = PlayAnimation(dwAnimationID, nAnimationState, RL_ANIMATION_UP_NONE, RL_ANIMATION_DOWN_UNKNOWN, m_AnimationStatus.MonoStatus.MonoParam.nOffsetTime);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::ResetAnimation()
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwAnimationID = 0;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    RL_ANIMATION_STATE nAnimationState = RL_ANIMATION_STATE_HOLD_ONCE;
    
    memset(&m_UpBodyLowAnimationTrack, 0, sizeof(m_UpBodyLowAnimationTrack));
    memset(&m_AnimationStatus.SplitStatus.UpLowTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.UpLowTrackParam));

    memset(&m_UpBodyHighAnimationTrack, 0, sizeof(m_UpBodyHighAnimationTrack));
    memset(&m_AnimationStatus.SplitStatus.UpHighTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.UpHighTrackParam));

    memset(&m_DownBodyLowAnimationTrack, 0, sizeof(m_DownBodyLowAnimationTrack));
    memset(&m_AnimationStatus.SplitStatus.DownLowTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.DownLowTrackParam));

    memset(&m_DownBodyHighAnimationTrack, 0, sizeof(m_DownBodyHighAnimationTrack));
    memset(&m_AnimationStatus.SplitStatus.DownHighTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.DownHighTrackParam));

    m_dwSerialAnimationID = 0;

    m_nDownBodyState = GetRLAnimationDown(
        m_FrameData.m_Current.nMoveState, 
        m_FrameData.m_Current.nOTAction,
        m_FrameData.m_Current.nJumpCount,
        RL_TURNING_STAND_NONE);

    if (m_nDownBodyState == RL_ANIMATION_DOWN_AUTOFLY)
    {
        if (m_pRLVehicle)
        {
            dwAnimationID = m_pRLVehicle->m_dwIdleAnimationID;

            nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_HOLD_ONCE, RL_ANIMATION_UP_NONE, RL_ANIMATION_DOWN_UNKNOWN);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        else
        {
            m_nDownBodyState = RL_ANIMATION_DOWN_NONE;
        }
    }
    else
    {
        if (!m_EquipResource.bSplitModel && m_nDownBodyState == RL_ANIMATION_DOWN_STAND)
        {
            // npc idle 的时候循环播放
            nAnimationState = RL_ANIMATION_STATE_HOLD_LOOP;
        }

        nAnimationTable = GetAnimationTable(m_nDownBodyState, m_AnimationStatus.nAdvance, m_nFightFlag);

        dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

        nRetCode = PlayAnimation(dwAnimationID, nAnimationState, RL_ANIMATION_UP_NONE, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpBodyHighTrackAnimationFinish()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nOffsetTime = 0;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    DWORD dwAnimationID = 0;

    ASSERT(m_EquipResource.bSplitModel);

    KG_PROCESS_SUCCESS(m_UpBodyHighAnimationTrack.nState == RL_ANIMATION_STATE_HOLD_ONCE);

    switch (m_nUpBodyState)
    {
    case RL_ANIMATION_UP_NONE:
    case RL_ANIMATION_UP_BEGIN_PICK_DOODAD:
    case RL_ANIMATION_UP_PICKING_DOODAD:
    case RL_ANIMATION_UP_END_SHEATH:
    case RL_ANIMATION_UP_END_UNSHEATH:
    case RL_ANIMATION_UP_END_PICK_DOODAD:
    case RL_ANIMATION_UP_DODGE:
    case RL_ANIMATION_UP_BEHIT:
    case RL_ANIMATION_UP_SKILL_PREPARE:
    case RL_ANIMATION_UP_SKILL_CHANNEL:
    case RL_ANIMATION_UP_RECIPE_PREPARE:
    case RL_ANIMATION_UP_OPEN_DOODAD:
        m_nUpBodyState = m_UpBodyLowAnimationTrack.nAnimationUpType;

        m_pUpBodyCurrentAnimationTrack = &m_UpBodyLowAnimationTrack;

        if (m_pUpBodyCurrentAnimationTrack->dwAnimationID)
            UpdateWeaponPosition(m_pUpBodyCurrentAnimationTrack->nSheathSocket, m_bSheath);

        memset(&m_UpBodyHighAnimationTrack, 0, sizeof(m_UpBodyHighAnimationTrack));
        memset(&m_AnimationStatus.SplitStatus.UpHighTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.UpHighTrackParam));
        break;
    case RL_ANIMATION_UP_BEGIN_SHEATH:
        ResetRLAnimationCommonInfo(m_UpBodyHighAnimationTrack);

        nAnimationTable = GetSheathAnimationTable(m_EquipResource.Player.nSheathSocket, TRUE, 1);

        dwAnimationID = GetAnimationID(nAnimationTable, FALSE);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_END_SHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_UP_BEGIN_UNSHEATH:
        ResetRLAnimationCommonInfo(m_UpBodyHighAnimationTrack);

        nAnimationTable = GetSheathAnimationTable(m_EquipResource.Player.nSheathSocket, FALSE, 1);

        dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_END_UNSHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_UP_SERIAL_BEGIN:
    case RL_ANIMATION_UP_SERIAL_END:
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    default:
        ASSERT(0);
        break;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::UpBodyLowTrackAnimationFinish()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nOffsetTime = 0;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    DWORD dwAnimationID = 0;

    ASSERT(m_EquipResource.bSplitModel);

    KG_PROCESS_SUCCESS(m_UpBodyLowAnimationTrack.nState == RL_ANIMATION_STATE_HOLD_ONCE);

    switch (m_nUpBodyState)
    {
    case RL_ANIMATION_UP_NONE:
        ResetRLAnimationCommonInfo(m_UpBodyLowAnimationTrack);

        if (m_UpBodyLowAnimationTrack.nState == RL_ANIMATION_STATE_IMPULSE)
        {
            nAnimationTable = GetAnimationTable(m_nDownBodyState, m_AnimationStatus.nAdvance, m_nFightFlag);
            dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

            nRetCode = PlaySplitAnimation(ANI_JOINT_TOP, dwAnimationID, RL_ANIMATION_STATE_HOLD_ONCE, RL_ANIMATION_UP_NONE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        break;
    case RL_ANIMATION_UP_BEGIN_SHEATH:
        ResetRLAnimationCommonInfo(m_DownBodyHighAnimationTrack);

        nAnimationTable = GetSheathAnimationTable(m_EquipResource.Player.nSheathSocket, TRUE, 1);

        dwAnimationID = GetAnimationID(nAnimationTable, FALSE);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_END_SHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_UP_BEGIN_UNSHEATH:
        ResetRLAnimationCommonInfo(m_DownBodyHighAnimationTrack);

        nAnimationTable = GetSheathAnimationTable(m_EquipResource.Player.nSheathSocket, FALSE, 1);

        dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

        nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_END_UNSHEATH, RL_ANIMATION_DOWN_UNKNOWN);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        break;
    case RL_ANIMATION_UP_BEGIN_PICK_DOODAD:
    case RL_ANIMATION_UP_PICKING_DOODAD:
        break;
    case RL_ANIMATION_UP_END_SHEATH:
    case RL_ANIMATION_UP_END_UNSHEATH:
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    case RL_ANIMATION_UP_END_PICK_DOODAD:
    case RL_ANIMATION_UP_DODGE:
    case RL_ANIMATION_UP_BEHIT:
    case RL_ANIMATION_UP_SKILL_PREPARE:
    case RL_ANIMATION_UP_SKILL_CHANNEL:
    case RL_ANIMATION_UP_OPEN_DOODAD:
        m_nUpBodyState = RL_ANIMATION_UP_NONE;
        ResetRLAnimationCommonInfo(m_UpBodyLowAnimationTrack);

        if (m_nDownBodyState == RL_ANIMATION_DOWN_AUTOFLY)
        {
            nRetCode = ResetAnimation();
            KGLOG_PROCESS_ERROR(nRetCode);
        }
        break;
    case RL_ANIMATION_UP_SERIAL_BEGIN:
        {
            KRLSerialAnimation SerialAnimation;

            nRetCode = GetCharacterSerialAnimation(m_EquipResource.nObject == RL_OBJECT_PLAYER, m_dwSerialAnimationID, &SerialAnimation);
            if (nRetCode && SerialAnimation.dwPhaseAnimationIDs[1])   // RL_ANIMATION_UP_SERIAL_PROCESSING
            {
                // TODO: will be removed

                KRLAdjustAnimationParam Param;

                memset(&m_UpBodyLowAnimationTrack, 0, sizeof(m_UpBodyLowAnimationTrack));

                nRetCode = FillRLAdjustAnimationParam(&Param, m_FrameData.m_Current.bSheathFlag);
                KGLOG_PROCESS_ERROR(nRetCode);

                dwAnimationID = GetAdjustAnimationIndexID(SerialAnimation.dwPhaseAnimationIDs[1], &Param);

                nRetCode = PlayAnimation(dwAnimationID, RL_ANIMATION_STATE_IMPULSE, RL_ANIMATION_UP_SERIAL_PROCESSING, RL_ANIMATION_DOWN_UNKNOWN);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                nRetCode = ResetAnimation();
                KGLOG_PROCESS_ERROR(nRetCode);
            }
        }
        break;
    case RL_ANIMATION_UP_SERIAL_END:
        nRetCode = ResetAnimation();
        KGLOG_PROCESS_ERROR(nRetCode);

        break;
    default:
        ASSERT(0);
        break;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::DownBodyHighTrackAnimationFinish()
{
    ASSERT(m_EquipResource.bSplitModel);

    m_nDownBodyState = m_DownBodyLowAnimationTrack.nAnimationDownType;

    m_pDownBodyCurrentAnimationTrack = &m_DownBodyLowAnimationTrack;

    memset(&m_DownBodyHighAnimationTrack, 0, sizeof(m_DownBodyHighAnimationTrack));
    memset(&m_AnimationStatus.SplitStatus.DownHighTrackParam, 0, sizeof(m_AnimationStatus.SplitStatus.DownHighTrackParam));

    return true;
}

int KRLCharacter::DownBodyLowTrackAnimationFinish()
{
    int nRetCode = false;
    int nResult = false;

    ASSERT(m_EquipResource.bSplitModel);

    KG_PROCESS_SUCCESS(m_DownBodyLowAnimationTrack.nState == RL_ANIMATION_STATE_HOLD_ONCE);

    ResetRLAnimationCommonInfo(m_DownBodyLowAnimationTrack);

    if (m_DownBodyHighAnimationTrack.nState == RL_ANIMATION_STATE_IMPULSE)
    {
        m_AnimationStatus.SplitStatus.DownHighTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.UpHighTrackParam.fStartTime);

        nRetCode = PlaySplitAnimation(ANI_JOINT_BOTTOM, m_UpBodyHighAnimationTrack.dwAnimationID, 
            m_UpBodyHighAnimationTrack.nState, 
            RL_ANIMATION_DOWN_UNKNOWN, 
            m_AnimationStatus.SplitStatus.DownHighTrackParam.nOffsetTime);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else if (m_DownBodyLowAnimationTrack.nState == RL_ANIMATION_STATE_IMPULSE)
    {
        m_AnimationStatus.SplitStatus.DownLowTrackParam.nOffsetTime = GetAnimationPlayTime(m_AnimationStatus.SplitStatus.UpLowTrackParam.fStartTime);

        nRetCode = PlaySplitAnimation(ANI_JOINT_BOTTOM, m_UpBodyLowAnimationTrack.dwAnimationID, 
            m_UpBodyLowAnimationTrack.nState, 
            RL_ANIMATION_DOWN_UNKNOWN, 
            m_AnimationStatus.SplitStatus.DownLowTrackParam.nOffsetTime);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::WholeBodyHighTrackAnimationFinish()
{
//    ASSERT(!m_EquipResource.bSplitModel);

    KG_PROCESS_SUCCESS(m_nDownBodyState == RL_ANIMATION_DOWN_DEATH);

    m_nUpBodyState = RL_ANIMATION_UP_NONE;
    m_nDownBodyState = RL_ANIMATION_DOWN_NONE;

    m_pUpBodyCurrentAnimationTrack = &m_UpBodyLowAnimationTrack;

    m_pDownBodyCurrentAnimationTrack = &m_DownBodyLowAnimationTrack;

    if (m_pDownBodyCurrentAnimationTrack->dwAnimationID)
        UpdateNpcModel(m_pDownBodyCurrentAnimationTrack->dwAnimationID);

    memset(&m_UpBodyHighAnimationTrack, 0, sizeof(m_UpBodyHighAnimationTrack));
    memset(&m_DownBodyHighAnimationTrack, 0, sizeof(m_DownBodyHighAnimationTrack));

Exit1:
    return true;
}

int KRLCharacter::WholeBodyLowTrackAnimationFinish()
{
    int nRetCode = false;
    int nResult = false;
    KRLSerialAnimation SerialAnimation;
    RL_ANIMATION_TABLE nAnimationTable = RL_ANIMATION_TABLE_NONE;
    DWORD dwAnimationID = 0;
    BOOL bSwim = FALSE;

//    ASSERT(!m_EquipResource.bSplitModel);

    switch (m_DownBodyLowAnimationTrack.nState)
    {
    case RL_ANIMATION_STATE_IMPULSE:
        switch (m_nUpBodyState)
        {
        case RL_ANIMATION_UP_NONE:
        case RL_ANIMATION_UP_DODGE:
        case RL_ANIMATION_UP_BEHIT:
            nRetCode = ResetAnimation();
            KGLOG_PROCESS_ERROR(nRetCode);

            break;
        case RL_ANIMATION_UP_SERIAL_BEGIN:
            ASSERT(m_EquipResource.nObject == RL_OBJECT_NPC);

            nRetCode = GetCharacterSerialAnimation(FALSE, m_dwSerialAnimationID, &SerialAnimation);
            if (nRetCode && SerialAnimation.dwPhaseAnimationIDs[1])   // RL_ANIMATION_UP_SERIAL_PROCESSING
            {
                nRetCode = PlayAnimation(SerialAnimation.dwPhaseAnimationIDs[1], 
                    RL_ANIMATION_STATE_HOLD_ONCE, RL_ANIMATION_UP_SERIAL_PROCESSING, RL_ANIMATION_DOWN_UNKNOWN);
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                nRetCode = ResetAnimation();
                KGLOG_PROCESS_ERROR(nRetCode);
            }
            break;
        case RL_ANIMATION_UP_SERIAL_PROCESSING:
            break;
        case RL_ANIMATION_UP_SERIAL_END:
            nRetCode = ResetAnimation();
            KGLOG_PROCESS_ERROR(nRetCode);

            break;
        default:
            break;
        }
        break;
    case RL_ANIMATION_STATE_HOLD_ONCE:
        break;
    case RL_ANIMATION_STATE_HOLD_LOOP:
        ASSERT(m_nDownBodyState == RL_ANIMATION_DOWN_STAND || m_nDownBodyState == RL_ANIMATION_DOWN_NONE);

        memset(&m_UpBodyLowAnimationTrack, 0, sizeof(m_UpBodyLowAnimationTrack));

        m_DownBodyLowAnimationTrack.Init(FALSE);

        nAnimationTable = GetAnimationTable(m_nDownBodyState, m_AnimationStatus.nAdvance, m_nFightFlag);

        dwAnimationID = GetAnimationID(nAnimationTable, m_FrameData.m_Current.bSheathFlag);

        nRetCode = PlayAnimation(dwAnimationID, m_DownBodyLowAnimationTrack.nState, RL_ANIMATION_UP_NONE, m_nDownBodyState);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        ASSERT(0);
        break;
    };

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::InitRLControl()
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pCharacter);

    if (m_EquipResource.nObject == RL_OBJECT_PLAYER)    // TODO: Use m_EquipResource.bSplitModel instead
    {
        m_RLControl.pUpdateWeapon = &KRLCharacter::UpdatePlayerWeaponSocket;
        m_RLControl.pUpdateAnimationSpeed = &KRLCharacter::UpdatePlayerAnimationSpeed;
        m_RLControl.pUpdateSheath = &KRLCharacter::UpdatePlayerSheath;
        m_RLControl.pPlayAnimation = &KRLCharacter::PlayPlayerAnimation;
        m_RLControl.pTurnFaceBegin = &KRLCharacter::TurnPlayerFaceBegin;
        m_RLControl.pTurnFaceEnd = &KRLCharacter::TurnPlayerFaceEnd;

        m_AnimationStatus.nType = RL_ANIMATION_SPLIT;
    }
    else
    {
        m_RLControl.pUpdateWeapon = NULL;
        m_RLControl.pUpdateAnimationSpeed = &KRLCharacter::UpdateNpcAnimationSpeed;
        m_RLControl.pUpdateSheath = NULL;
        m_RLControl.pPlayAnimation = &KRLCharacter::PlayNpcAnimation;
        m_RLControl.pTurnFaceBegin = &KRLCharacter::TurnNpcFaceBegin;
        m_RLControl.pTurnFaceEnd = &KRLCharacter::TurnNpcFaceEnd;

        m_AnimationStatus.nType = RL_ANIMATION_MONO;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLCharacter::FillRLAdjustAnimationParam(KRLAdjustAnimationParam* pParam, BOOL bSheathFlag) const
{
    int nRetCode = false;
    int nResult = false;
    D3DXVECTOR3 vPosition;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;

    KGLOG_PROCESS_ERROR(pParam);

    pParam->bAboveWater = m_FrameData.m_Current.bAboveWater;
    pParam->bJump = m_FrameData.m_Current.nMoveState == cmsOnJump || m_FrameData.m_Current.nMoveState == cmsOnSwimJump; // TODO: error
    pParam->bRide = m_pRLHorse != NULL;
    pParam->bSpeedup = m_FrameData.m_Current.nVelocityXY >= g_pRL->m_TableList.GetNumber(RL_NUMBER_CHARACTER_RIDE_RUN_FASTER_SPEED);

    pParam->bSwim = 
        m_nDownBodyState == RL_ANIMATION_DOWN_SWIM ||
        m_nDownBodyState == RL_ANIMATION_DOWN_SWIM_JUMP ||
        m_nDownBodyState == RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP ||
        m_nDownBodyState == RL_ANIMATION_DOWN_FLOAT;

    pParam->CharacterParam.dwModelID = m_EquipResource.dwModelID;

    if (m_EquipResource.nObject == RL_OBJECT_PLAYER)
    {
        pParam->CharacterParam.bPlayer = TRUE;
        pParam->CharacterParam.nRoleType = m_EquipResource.Player.nRoleType;

        pParam->nWeaponType = m_EquipResource.Player.nWeaponType;

        if (bSheathFlag)
        {
            pParam->nWeaponType = RL_WEAPON_NONE;
        }
    }
    else
    {
        pParam->CharacterParam.bPlayer = FALSE;
        pParam->CharacterParam.nRoleType = rtInvalid;
    }

    memcpy(pParam->dwSkillBuffs, m_FrameData.m_Current.dwSkillBuffs, sizeof(pParam->dwSkillBuffs));

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    GetPosition(vPosition);

    pParam->dwTerrainID = p3DScene->GetGroundType(vPosition);

    nResult = true;
Exit0:
    return nResult;
}

#pragma warning(pop)