#include "stdafx.h"
#include "./krllocalcharacter.h"
#include "./krlrides.h"
#include "../gameworld/krlanimationtable.h"
#include "../../SO3Represent.h"
#include "IKG3DSoundShell.h"
#include "SO3World/IKGSO3WorldClient.h"

void KRLLocalCharacter::Init()
{
    m_nTurning = RL_TURNING_STAND_NONE;

    m_dwLockControlFrame = 0;

    m_fFaceYaw = 0.0f;
    m_fFaceExpectYaw = 0.0f;
    m_fPreviousFaceYaw = 0.0f;

    m_fFacePitch = 0.0f;

    m_fFootYaw = 0.0f;
    m_fFootExpectYaw = 0.0f;
    m_fPreviousFootYaw = 0.0f;

    m_fFootPitch = 0.0f;
    m_fFootExpectPitch = 0.0f;

    m_fFootRoll = 0.0f;
    m_fFootExpectRoll = 0.0f;

    m_bSheath = FALSE;
    m_nAnimationUp = RL_ANIMATION_UP_UNKNOWN;
}

void KRLLocalCharacter::Reset()
{
    m_RLCharacter.Reset();
}

int KRLLocalCharacter::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RL_ANIMATION_UP nAnimationUp = RL_ANIMATION_UP_NONE;
    RL_ANIMATION_DOWN nAnimationDown = RL_ANIMATION_DOWN_NONE;
    RL_ADVANCE nAdvance = RL_ADVANCE_FORWARD;

    ASSERT(!m_RLCharacter.IsFadeOut());

    if (m_RLCharacter.m_RenderData.HasLoadedMDL())
    {
        if (bFrame)
        {
            m_RLCharacter.m_FrameData.UpdateRepresentFrameData(TRUE);

            if (m_bSheath != m_RLCharacter.m_FrameData.m_Current.bSheathFlag)
            {
                m_bSheath = m_RLCharacter.m_FrameData.m_Current.bSheathFlag;
                m_RLCharacter.UpdateSheath(m_bSheath);
            }
        }

        nRetCode = m_RLCharacter.m_FrameData.Interpolate(m_RLCharacter.m_RenderData.IsVisible(), false);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdatePosition();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateDirection();
        KGLOG_PROCESS_ERROR(nRetCode);
        
        nAnimationDown = GetRLAnimationDown(
            m_RLCharacter.m_FrameData.m_Current.nMoveState, 
            m_RLCharacter.m_FrameData.m_Current.nOTAction,
            m_RLCharacter.m_FrameData.m_Current.nJumpCount,
            m_nTurning);

        nAdvance = GetAdvance(nAnimationDown);

        nRetCode = UpdateListener();
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bFrame)
        {
            if (m_RLCharacter.m_bAnimationPaused && nAnimationDown != RL_ANIMATION_DOWN_FREEZE)
                m_RLCharacter.PauseAnimation(FALSE);
            
            nAnimationUp = GetRLAnimationUp(m_RLCharacter.m_FrameData.m_Current.nOTAction);

            if (nAnimationUp == RL_ANIMATION_UP_NONE && m_nAnimationUp != RL_ANIMATION_UP_NONE || 
                nAnimationUp != RL_ANIMATION_UP_NONE)
            {
                nRetCode = m_RLCharacter.UpdateAnimationUp(nAnimationUp);
                KGLOG_PROCESS_ERROR(nRetCode);

                m_nAnimationUp = nAnimationUp;
            }

            nRetCode = m_RLCharacter.UpdateVehicle(
                m_RLCharacter.m_FrameData.m_Current.nVehicleTrack, 
                FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = m_RLCharacter.UpdateAnimationDown(nAnimationDown, nAdvance, 
                m_RLCharacter.m_FrameData.m_Current.nFightFlag,
                m_RLCharacter.m_FrameData.m_Current.dwSkillBuffs);
            KGLOG_PROCESS_ERROR(nRetCode);

            UpdateLifeState();

            UpdateAnimationSpeed(nAnimationDown, nAdvance);

            if (m_dwLockControlFrame > 0)
            {
                --m_dwLockControlFrame;
            }

            hr = m_RLCharacter.m_RenderData.Update();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacter::UpdateDisplayData(BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacterEquipResource EquipResource;
    KRLCharacterDisplayData RLCharacterDisplayData;

    nRetCode = m_RLCharacter.PauseAnimationStatus(&RLCharacterDisplayData);
    KGLOG_CHECK_ERROR(nRetCode);

    nRetCode = m_RLCharacter.m_FrameData.UpdateCurrentRepresentFrameData(true);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.m_FrameData.Interpolate(m_RLCharacter.m_RenderData.IsVisible(), true);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdatePosition();
    KGLOG_PROCESS_ERROR(nRetCode);
    
    nRetCode = UpdateDirection();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterModel(m_RLCharacter.m_FrameData.m_pCharacter, &EquipResource);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateModel(EquipResource, bForceUpdate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateVehicle(m_RLCharacter.m_FrameData.m_Current.nVehicleTrack, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.ResumeAnimationStatus(&RLCharacterDisplayData);
    KGLOG_CHECK_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateTitleMark();
    KGLOG_PROCESS_ERROR(nRetCode);

    UpdateSkillBuff();

    m_bSheath = m_RLCharacter.m_FrameData.m_Current.bSheathFlag;
    m_RLCharacter.UpdateWeaponSocketPosition(m_bSheath);

    nResult = true;
Exit0:
    return nResult;
}

void KRLLocalCharacter::SetFaceYaw(float fFaceYaw)
{
    m_fPreviousFaceYaw = m_fFaceYaw;

    m_fFaceYaw = fFaceYaw;
    m_fFaceExpectYaw = fFaceYaw;
}

void KRLLocalCharacter::SetFaceExpectYaw(float fFaceYaw)
{
    m_fFaceExpectYaw = fFaceYaw;
}

void KRLLocalCharacter::SetFacePitch(float fFacePitch)
{
    m_fFacePitch = fFacePitch;
}

void KRLLocalCharacter::SetFootYaw(float fFootYaw)
{
    m_fFootYaw = fFootYaw;
    m_fFootExpectYaw = fFootYaw;
}

void KRLLocalCharacter::SetFootExpectYaw(float fFootYaw)
{
    KRLCharacterFrameData& FrameData = m_RLCharacter.m_FrameData;

    CHARACTER_MOVE_STATE nMoveState = FrameData.m_Current.nMoveState;

    if (!((nMoveState == cmsOnJump || nMoveState == cmsOnSwimJump) && 
        FrameData.m_Previous.nJumpCount == FrameData.m_Current.nJumpCount))
    {
        m_fFootExpectYaw = fFootYaw;
    }
}

int KRLLocalCharacter::UpdateFaceFootDirection(RL_TURNING nTurning)
{
    int nRetCode = false;
    int nResult = false;
    float fFaceYaw = 0.0f;
    float fFootYaw = 0.0f;
    float fYawTurnResetSpeed = 0.0f;
    float fYawTurnResetDelta = 0.0f;
    float fYawTurnSpeed = 0.0f;
    float fYawTurnDelta = 0.0f;

    fYawTurnResetSpeed = GetYawTurnResetSpeed(m_RLCharacter.m_FrameData.m_Current.bOnRide);
    fYawTurnResetDelta = fYawTurnResetSpeed * (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

    fFaceYaw = CloseToDirection(m_fFaceExpectYaw, m_fFaceYaw, fYawTurnResetDelta);

    switch (nTurning)
    {
    case RL_TURNING_STAND_NONE:
        fFootYaw = m_fFootYaw;
        break;
    case RL_TURNING_STAND_FACE_TURN_LEFT:
    case RL_TURNING_STAND_FACE_TURN_RIGHT:
        fFootYaw = m_fFootYaw;
        break;
    case RL_TURNING_STAND_FOOT_TURN_LEFT:
    case RL_TURNING_STAND_FOOT_TURN_RIGHT:
        fFootYaw = CloseToDirection(fFaceYaw, m_fFootYaw, fYawTurnResetDelta);
        fFootYaw = ClampDirection(fFaceYaw, fFootYaw, D3DX_PI / 2.01f);
        break;
    case RL_TURNING_MOVING_NONE:
    case RL_TURNING_MOVING_FACE_TURN_LEFT:
    case RL_TURNING_MOVING_FACE_TURN_RIGHT:
        fYawTurnSpeed = GetYawTurnSpeed(m_RLCharacter.m_FrameData.m_Current.bOnRide);
        fYawTurnDelta = fYawTurnSpeed * (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

        fFootYaw = CloseToDirection(m_fFootExpectYaw, m_fFootYaw, fYawTurnDelta);
        fFootYaw = ClampDirection(fFaceYaw, fFootYaw, D3DX_PI / 2.01f);
        break;
    }

    nRetCode = SetFaceFootDirection(fFaceYaw, m_fFacePitch, fFootYaw);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_fPreviousFaceYaw = m_fFaceYaw;
    m_fPreviousFootYaw = m_fFootYaw;

    m_fFaceYaw = fFaceYaw;
    m_fFootYaw = fFootYaw;

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacter::UpdateDirection()
{
    int nRetCode = false;
    int nResult = false;
    BOOL bMoving = FALSE;
    CHARACTER_MOVE_STATE nMoveState = cmsInvalid;

    nMoveState = m_RLCharacter.m_FrameData.m_Current.nMoveState;

    bMoving = nMoveState != cmsOnStand;

    m_nTurning = GetTurning(m_nTurning, bMoving, m_fFaceYaw, m_fFootYaw, m_fPreviousFaceYaw, m_fPreviousFootYaw);

    nRetCode = UpdateFaceFootDirection(m_nTurning);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacter::UpdatePosition()
{
    int nResult = false;
    D3DXVECTOR3 vPosition;

    vPosition = m_RLCharacter.m_FrameData.GetCurrentPosition();

	m_RLCharacter.SetPosition(vPosition);

    nResult = true;
    return nResult;
}

int KRLLocalCharacter::SetFaceFootDirection(float fFaceYaw, float fFacePitch, float fFootYaw)
{
    int nRetCode = false;
    int nResult = false;
    KRLRides* pRides = NULL;
    D3DXQUATERNION qRotationLH;
    float fDelta = 0.0f;

    if (m_RLCharacter.m_pRLHorse)
        pRides = m_RLCharacter.m_pRLHorse;
    else if (m_RLCharacter.m_pRLVehicle)
        pRides = m_RLCharacter.m_pRLVehicle;

    if (pRides)
    {
        m_RLCharacter.KRLSceneObject::SetDirectionLH(D3DX_PI);
        if (GetSlopeRotation(qRotationLH, (RL_TERRAIN_SLOPE)pRides->m_dwSlopeType))
        {
            pRides->SetDirectionLH(qRotationLH);
        }
    }
    else
    {
        if (m_RLCharacter.m_FrameData.m_Current.nMoveState == cmsOnSwim)
        {
            if (!m_RLCharacter.m_FrameData.m_Current.bUnderWater)
                fFacePitch = 0.0f;
			

            D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fFaceYaw), fFacePitch, 0.0f);
            m_RLCharacter.m_RenderData.SetRotation(qRotationLH);
        }
        else
        {
            if (m_RLCharacter.m_EquipResource.bSplitModel)
            {
                m_RLCharacter.KRLSceneObject::SetDirectionLH(fFootYaw);
            }
            else
            {
                if (GetSlopeRotation(qRotationLH, m_RLCharacter.m_EquipResource.nTerrainSlope))
                {
                    m_RLCharacter.SetDirectionLH(qRotationLH);
                }
            }
        }
    }

    fDelta = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fFootYaw);
    m_RLCharacter.TurnFace(fDelta);

    nResult = true;
    return nResult;
}

int KRLLocalCharacter::GetSlopeRotation(D3DXQUATERNION& qRotationLH, RL_TERRAIN_SLOPE nTerrainSlope)
{
    static float const PITCH_STEP = D3DX_PI / 600.0f;
    static float const ROLL_STEP = D3DX_PI / 600.0f;

    int nRetCode = false;
    int nResult = false;
    float fTerrainYawLH = m_fFootYaw;
    float fTerrainRollLH = 0.0f;
    float fTerrainPitchLH = 0.0f;
    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR3 vPosition;

    KG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_Current.nMoveState != cmsOnJump);
    KG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_Current.nMoveState != cmsOnSwimJump);

    p3DScene = m_RLCharacter.m_RenderData.GetRLScene()->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    m_RLCharacter.GetPosition(vPosition);

    nRetCode = GetTerrainSlopeLean(p3DScene, vPosition, fTerrainYawLH, nTerrainSlope, fTerrainPitchLH, fTerrainRollLH);
    KG_PROCESS_ERROR(nRetCode);

    if (abs(m_fFootExpectPitch - fTerrainPitchLH) > D3DX_PI / 1800.0f)
        m_fFootExpectPitch = fTerrainPitchLH;
    m_fFootPitch = CloseToPitch(m_fFootExpectPitch, m_fFootPitch, PITCH_STEP);

    if (abs(m_fFootExpectRoll - fTerrainRollLH) > D3DX_PI / 1800.0f)
        m_fFootExpectRoll = fTerrainRollLH;
    m_fFootRoll = CloseToRoll(m_fFootExpectRoll, m_fFootRoll, ROLL_STEP);

    switch (nTerrainSlope)
    {
    case RL_TERRAIN_SLOPE_NONE:
        YawToQuatLH(qRotationLH, YawLHToModelLH(fTerrainYawLH));
        break;
    case RL_TERRAIN_SLOPE_LENGTHWAYS:
        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), m_fFootPitch, 0.0f);
        break;
    case RL_TERRAIN_SLOPE_TRANSVERSE:
        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), 0.0f, m_fFootRoll);
        break;
    case RL_TERRAIN_SLOPE_ALL:
        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), m_fFootPitch, m_fFootRoll);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

void KRLLocalCharacter::UpdateAnimationSpeed(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance)
{
    float fSpeed = 1.0f;

    fSpeed = GetAnimationSpeed(
        nAnimationDown, 
        nAdvance, 
        m_RLCharacter.m_FrameData.m_Current.nVelocityXY, 
        m_RLCharacter.m_FrameData.m_Current.bOnRide);

    m_RLCharacter.UpdateAnimationSpeed(fSpeed / m_RLCharacter.m_EquipResource.MDL.fMDLScale);
}

RL_ADVANCE KRLLocalCharacter::GetAdvance(RL_ANIMATION_DOWN nAnimationDown)
{
    RL_ADVANCE nAdvance = RL_ADVANCE_FORWARD;
    RL_ADVANCE_TYPE nAdvanceType = RL_ADVANCE_TYPE_COUNT;
    float fFootFaceDeltaYaw = 0.0f;
    
    switch (nAnimationDown)
    {
    case RL_ANIMATION_DOWN_NONE:
    case RL_ANIMATION_DOWN_STAND:
    case RL_ANIMATION_DOWN_TURN_LEFT:
    case RL_ANIMATION_DOWN_TURN_RIGHT:
    case RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE:
    case RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL:
    case RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE:
    case RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE:
    case RL_ANIMATION_DOWN_OTACTION_PICKING:
    case RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL:
    case RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE:
    case RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL:
        break;
    case RL_ANIMATION_DOWN_WALK:
    case RL_ANIMATION_DOWN_RUN:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
    case RL_ANIMATION_DOWN_SWIM:
    case RL_ANIMATION_DOWN_SWIM_JUMP:
    case RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP:
    case RL_ANIMATION_DOWN_FLOAT:
        nAdvanceType = RL_ADVANCE_TYPE_SWIM;
        break;
    case RL_ANIMATION_DOWN_JUMP:
    case RL_ANIMATION_DOWN_DOUBLE_JUMP:
    case RL_ANIMATION_DOWN_SIT_DOWN:
    case RL_ANIMATION_DOWN_DEATH:
    case RL_ANIMATION_DOWN_RISE:
        break;
    case RL_ANIMATION_DOWN_DASH:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
    case RL_ANIMATION_DOWN_KNOCK_DOWN:
    case RL_ANIMATION_DOWN_KNOCK_BACK:
    case RL_ANIMATION_DOWN_KNOCK_OFF:
    case RL_ANIMATION_DOWN_HALT:
    case RL_ANIMATION_DOWN_FREEZE:
    case RL_ANIMATION_DOWN_ENTRAP:
        break;
    case RL_ANIMATION_DOWN_AUTOFLY:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
    case RL_ANIMATION_DOWN_PULL:
    case RL_ANIMATION_DOWN_REPULSED:
    case RL_ANIMATION_DOWN_SKID:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
    default:
        ASSERT(0);
        break;
    }

    KG_PROCESS_SUCCESS(nAdvanceType == RL_ADVANCE_TYPE_COUNT);

    fFootFaceDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(
        m_RLCharacter.m_FrameData.m_Current.fSubjectiveDirection - m_RLCharacter.m_FrameData.m_Current.fFaceDirection);

    nAdvance = ::GetAdvance(nAdvanceType, fFootFaceDeltaYaw);

Exit1:
    return nAdvance;
}

void KRLLocalCharacter::UpdateLifeState()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    KForceRelationModel const* pForceRelationModel = NULL;

    KGLOG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_pCharacter);

    KG_PROCESS_SUCCESS(!m_RLCharacter.m_FrameData.IsLifeChanged());

    KG_PROCESS_SUCCESS(!g_pRL->m_Option.bHeadSFXEnabled[CHARACTER_TYPE_LOCAL_PLAYER][CHARACTER_SFX_LIFE - CHARACTER_SFX_BBOX_BALLOON_BEGIN]);

    pForceRelationModel = GetForceRelationModel(m_RLCharacter.GetObjectID(), g_pRL->m_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pForceRelationModel);

    hr = m_RLCharacter.m_RenderData.SetPercentage(pForceRelationModel->dwColor, m_RLCharacter.m_FrameData.m_Current.fLifePercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}

int KRLLocalCharacter::UpdateSkillBuff()
{
    int nRetCode = false;
    int nResult = false;
    KSkillBuffParam SkillBuff[SKILL_BUFF_PART_COUNT];

    KGLOG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_pCharacter);

    m_RLCharacter.m_FrameData.UpdateSkillBuff();

    nRetCode = GetCharacterSkillBuff(m_RLCharacter.m_EquipResource, m_RLCharacter.m_FrameData.m_Current.dwSkillBuffs, SkillBuff);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateSkillBuff(SkillBuff, _countof(SkillBuff));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacter::PrepareCastSkill(DWORD dwSkillID, DWORD dwSkillLevel)
{
    int nRetCode = false;
    int nResult = false;
    KSkillCasterParam SkillCasterParam;

    KGLOG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_pCharacter);

    nRetCode = GetCharacterSkillCasterParam(m_RLCharacter.m_EquipResource, dwSkillID, dwSkillLevel, &SkillCasterParam);
    KG_PROCESS_ERROR(nRetCode);

    m_dwLockControlFrame = SkillCasterParam.dwLockControlTime;

    nRetCode = m_RLCharacter.PlayAnimation(SkillCasterParam.dwPrepareAnimationID, RL_ANIMATION_STATE_IMPULSE,
        RL_ANIMATION_UP_SKILL_PREPARE, RL_ANIMATION_DOWN_UNKNOWN);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLLocalCharacter::CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID)
{
    int nRetCode = false;
    int nResult = false;
    KSkillCasterParam SkillCasterParam;

    KGLOG_PROCESS_ERROR(m_RLCharacter.m_FrameData.m_pCharacter);

    nRetCode = GetCharacterSkillCasterParam(m_RLCharacter.m_EquipResource, dwSkillID, dwSkillLevel, &SkillCasterParam);
    KG_PROCESS_ERROR(nRetCode);

    m_dwLockControlFrame = SkillCasterParam.dwLockControlTime;

    nRetCode = m_RLCharacter.CastSkill(SkillCasterParam, dwSkillID, dwSkillLevel, dwBulletID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}


int KRLLocalCharacter::UpdateListener()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = NULL;
    float fFaceYaw = 0.0f;
    D3DXVECTOR3 vFront(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vFrontTop(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 vPosition(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vVelocity(0.0f, 0.0f, 0.0f);

    pSoundShell = g_pRL->m_pSound;
    KG_PROCESS_SUCCESS(pSoundShell == NULL);

    fFaceYaw = YawLHToRH(m_fFaceYaw);

    m_RLCharacter.GetPosition(vPosition);

    vPosition *= 0.01f;

    hr = pSoundShell->SetListenerPosition(&vPosition, &vVelocity);
    KGLOG_COM_PROCESS_ERROR(hr);

    vFront.x = cosf(fFaceYaw);
    vFront.y = 0.0f;
    vFront.z = sinf(fFaceYaw);

    hr = pSoundShell->SetOrientation(&vFront, &vFrontTop);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}