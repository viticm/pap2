#include "stdafx.h"
#include "./krlremotecharacter.h"
#include "./krlrides.h"
#include "../gameworld/krlanimationtable.h"
#include "../../common/ksmallobjectpool.h"
#include "../../SO3Represent.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/KNpcTemplateList.h"
#include "SO3World/KSO3World.h"
#include "../../Source/Common/SO3World/Src/KNpc.h"

namespace
{
    KSmallObjectPool g_Allocator(_T("KRLRemoteCharacter"), sizeof(KRLRemoteCharacter));
}

void* KRLRemoteCharacter::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
    return g_Allocator.Alloc(); 
}

void KRLRemoteCharacter::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
    g_Allocator.Free(pData); 
}

void* KRLRemoteCharacter::operator new(size_t /* nSize */)
{ 
    return g_Allocator.Alloc(); 
}

void KRLRemoteCharacter::operator delete(void* pData) throw()
{ 
    g_Allocator.Free(pData); 
}

void KRLRemoteCharacter::Init()
{
    m_nTurning = RL_TURNING_STAND_NONE;

    m_bVisible = FALSE;

    m_fFaceYaw = 0.0f;
    m_fFaceExpectYaw = 0.0f;
    m_fPreviousFaceYaw = 0.0f;

    m_fFacePitch = 0.0f;

    m_fFootYaw = 0.0f;
    m_fFootExpectYaw = 0.0f;

    m_fFootPitch = 0.0f;
    m_fFootExpectPitch = 0.0f;

    m_fFootRoll = 0.0f;
    m_fFootExpectRoll = 0.0f;

    m_fExpireTime = 0.0;

    m_bSheath = FALSE;
    m_nAnimationUp = RL_ANIMATION_UP_UNKNOWN;
}

int KRLRemoteCharacter::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RL_ANIMATION_DOWN nAnimationDown = RL_ANIMATION_DOWN_NONE;
    RL_ADVANCE nAdvance = RL_ADVANCE_FORWARD;

    if (m_RLCharacter.m_RenderData.HasLoadedMDL())
    {
        if (!m_RLCharacter.IsFadeOut() && m_FrameData.m_pCharacter)
        {
            if (bFrame)
            {
                m_FrameData.UpdateRepresentFrameData(
                    m_RLCharacter.m_RenderData.IsVisible(), 
                    m_FrameData.GetCurrentPosition());
            }

            nRetCode = m_FrameData.Interpolate(m_RLCharacter.m_RenderData.IsVisible(), false);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = UpdatePosition(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            if (bFrame)
            {
				nRetCode = UpdateYaw(&nAdvance);
				KGLOG_PROCESS_ERROR(nRetCode);
            }

            nRetCode = UpdateDirection(TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);

            if (bFrame)
            {
                nRetCode = UpdateSheath();
                KGLOG_PROCESS_ERROR(nRetCode);

	            nAnimationDown = GetRLAnimationDown(
	                m_FrameData.m_Current.nMoveState, 
	                m_FrameData.m_Current.nOTAction,
	                m_FrameData.m_Current.nJumpCount,
	                m_nTurning);

                if (m_RLCharacter.m_bAnimationPaused && nAnimationDown != RL_ANIMATION_DOWN_FREEZE)
                    m_RLCharacter.PauseAnimation(FALSE);

                nRetCode = UpdateAnimationUp();
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = m_RLCharacter.UpdateVehicle(
                    m_FrameData.m_Current.nVehicleTrack, 
                    FALSE);
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = m_RLCharacter.UpdateAnimationDown(nAnimationDown, nAdvance,
                    m_FrameData.m_Current.bSlip,
                    m_FrameData.m_Current.nFightFlag,
                    m_FrameData.m_Current.dwSkillBuffs);
                KGLOG_PROCESS_ERROR(nRetCode);

                UpdateLifeState(FALSE);
                
                UpdateAnimationSpeed(nAnimationDown, nAdvance);
            }
        }

        if (bFrame)
        {
            m_RLCharacter.UpdateFade();

            hr = m_RLCharacter.m_RenderData.Update();
            KGLOG_COM_PROCESS_ERROR(hr);

            if (!IS_PLAYER(m_RLCharacter.GetObjectID()))
            {
                BOOL bSelectable = m_RLCharacter.m_RenderData.IsMDLSelectable();

                if (nAnimationDown == RL_ANIMATION_DOWN_DEATH && bSelectable)
                {
                    nRetCode = m_RLCharacter.SetSelectable(FALSE);
                    KGLOG_PROCESS_ERROR(nRetCode);
                } 
                else if (nAnimationDown != RL_ANIMATION_DOWN_DEATH && 
                    !bSelectable && 
                    m_FrameData.m_pCharacter)
                {
                    nRetCode = m_RLCharacter.ResetSelectable();
                    KGLOG_PROCESS_ERROR(nRetCode);
                }
            }
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::UpdateDisplayData(BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLCharacterEquipResource EquipResource;
    KRLCharacterDisplayData RLCharacterDisplayData;

    nRetCode = m_RLCharacter.PauseAnimationStatus(&RLCharacterDisplayData);
    KGLOG_CHECK_ERROR(nRetCode);

    nRetCode = m_FrameData.UpdateCurrentRepresentFrameData(true, m_FrameData.GetCurrentPosition());
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterModel(&g_pRL->m_ResourceLimit, 
        m_FrameData.m_pCharacter, &EquipResource);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateModel(EquipResource, bForceUpdate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_FrameData.Interpolate(m_RLCharacter.m_RenderData.IsVisible(), true);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateVehicle(m_FrameData.m_Current.nVehicleTrack, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdatePosition(TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);
    
	nRetCode = UpdateYaw(NULL);
	KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateDirection(bForceUpdate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateBalloon();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateTitleMark();
    KGLOG_PROCESS_ERROR(nRetCode);

    UpdateSkillBuff();

    UpdateLifeState(TRUE);

    nRetCode = m_RLCharacter.ResumeAnimationStatus(&RLCharacterDisplayData);
    KGLOG_CHECK_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::UpdatePosition(BOOL bUpdateDisplayData)
{
    int nRetCode = false;
    int nResult = false;
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vPrevPosition;

    m_FrameData.UpdatePositionOffset();

    vPosition = m_FrameData.GetCurrentPosition();

    m_RLCharacter.GetPosition(vPrevPosition);

    if (D3DXVec3Length(&(vPosition - vPrevPosition)) > 0.5f || bUpdateDisplayData)
	    m_RLCharacter.SetPosition(vPosition);

    nRetCode = m_RLCharacter.UpdateTitleMarkPosition();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::UpdateFaceFootDirection(RL_TURNING nTurning, BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    float fFaceYaw = 0.0f;
    float fFootYaw = 0.0f;
    float fYawTurnResetSpeed = 0.0f;
    float fYawTurnResetDelta = 0.0f;
    float fYawTurnSpeed = 0.0f;
    float fYawTurnDelta = 0.0f;

    if (IS_PLAYER(m_RLCharacter.GetObjectID()))
    {
        fYawTurnResetSpeed = GetYawTurnResetSpeed(m_FrameData.m_Current.bOnRide);
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
            fYawTurnSpeed = GetYawTurnSpeed(m_FrameData.m_Current.bOnRide);
            fYawTurnDelta = fYawTurnSpeed * (float)(g_pRL->m_fTime - g_pRL->m_fTimeLast);

            fFootYaw = CloseToDirection(m_fFootExpectYaw, m_fFootYaw, fYawTurnDelta);
            fFootYaw = ClampDirection(fFaceYaw, fFootYaw, D3DX_PI / 2.01f);
            break;
        }

        nRetCode = SetFaceFootDirection(fFaceYaw, m_fFacePitch, fFootYaw, bForceUpdate);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        fFaceYaw = m_fFaceExpectYaw;
        fFootYaw = m_fFaceExpectYaw;

        nRetCode = SetFaceFootDirection(fFaceYaw, m_fFacePitch, fFootYaw, bForceUpdate);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    m_fPreviousFaceYaw = m_fFaceYaw;
    m_fPreviousFootYaw = m_fFootYaw;

    m_fFaceYaw = fFaceYaw;
    m_fFootYaw = fFootYaw;

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::UpdateYaw(RL_ADVANCE *pnAdvance)
{
    int nResult = false;
    RL_ADVANCE nAdvance = RL_ADVANCE_FORWARD;

    m_FrameData.UpdateCurrentDirection();

    nAdvance = GetAdvance();

    m_fFaceExpectYaw = m_FrameData.m_Current.fFaceDirection;

    m_fFootExpectYaw = m_FrameData.m_Current.fSubjectiveDirection;

    if (nAdvance == RL_ADVANCE_BACKWARD)
        m_fFootExpectYaw += D3DX_PI;

    if (pnAdvance)
        *pnAdvance = nAdvance;

	nResult = true;
	return nResult;
}

int KRLRemoteCharacter::UpdateDirection(BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    BOOL bMoving = FALSE;
    CHARACTER_MOVE_STATE nMoveState = cmsInvalid;

    nMoveState = m_FrameData.m_Current.nMoveState;

    bMoving = nMoveState != cmsOnStand;

    m_nTurning = GetTurning(m_nTurning, bMoving, m_fFaceYaw, m_fFootYaw, m_fPreviousFaceYaw, m_fPreviousFootYaw);

    nRetCode = UpdateFaceFootDirection(m_nTurning, bForceUpdate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::SetFaceFootDirection(float fFaceYaw, float fFacePitch, float fFootYaw, BOOL bForceUpdate)
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
        if (GetSlopeRotation(qRotationLH, (RL_TERRAIN_SLOPE)pRides->m_dwSlopeType, bForceUpdate))
        {
            pRides->SetDirectionLH(qRotationLH);
        }
    }
    else
    {
        if (m_FrameData.m_Current.nMoveState == cmsOnSwim)
        {
            if (!m_FrameData.m_Current.bUnderWater)
                fFacePitch = 0.0f;

            D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fFaceYaw), fFacePitch, 0.0f);
            m_RLCharacter.m_RenderData.SetRotation(qRotationLH);
        }
        else
        {
            if (m_RLCharacter.m_EquipResource.nObject == RL_OBJECT_PLAYER)
            {
                m_RLCharacter.KRLSceneObject::SetDirectionLH(fFootYaw);
            }
            else
            {
                if (GetSlopeRotation(qRotationLH, m_RLCharacter.m_EquipResource.nTerrainSlope, bForceUpdate))
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

RL_ADVANCE KRLRemoteCharacter::GetAdvance()
{
    RL_ADVANCE nAdvance = RL_ADVANCE_FORWARD;
    RL_ADVANCE_TYPE nAdvanceType = RL_ADVANCE_TYPE_COUNT;
    float fFootFaceDeltaYaw = 0.0f;
    
	switch (m_FrameData.m_Current.nMoveState)
    {
	case cmsInvalid:
	case cmsOnStand:
		 break;
	case cmsOnWalk:
	case cmsOnRun:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
		break;
	case cmsOnJump:
		break;
	case cmsOnSwimJump:
	case cmsOnSwim:
	case cmsOnFloat:
        nAdvanceType = RL_ADVANCE_TYPE_SWIM;
        break;
	case cmsOnSit:
	case cmsOnKnockedDown:
	case cmsOnKnockedBack:
	case cmsOnKnockedOff:
	case cmsOnHalt:
	case cmsOnFreeze:
	case cmsOnEntrap:
		break;
	case cmsOnAutoFly:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
	case cmsOnDeath:
		break;
	case cmsOnDash:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
	case cmsOnPull:
	case cmsOnRepulsed:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
	case cmsOnRise:
		break;
	case cmsOnSkid:
        nAdvanceType = RL_ADVANCE_TYPE_NORMAL;
        break;
	default:
		ASSERT(0);
		break;
	}

    KG_PROCESS_SUCCESS(nAdvanceType == RL_ADVANCE_TYPE_COUNT);

    fFootFaceDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(
        m_FrameData.m_Current.fSubjectiveDirection - m_FrameData.m_Current.fFaceDirection);

    nAdvance = ::GetAdvance(nAdvanceType, fFootFaceDeltaYaw);

Exit1:
    return nAdvance;
}

void KRLRemoteCharacter::UpdateLifeState(BOOL bUpdateDisplayData)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    BOOL bShowLifeBar = TRUE;
    int nCharacterType = CHARACTER_TYPE_REMOTE_PLAYER;
    KForceRelationModel const* pForceRelationModel = NULL;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pCharacter);

    KG_PROCESS_SUCCESS(!m_FrameData.IsLifeChanged() && !bUpdateDisplayData);

    if (!IS_PLAYER(m_RLCharacter.GetObjectID()))
    {
        KNpc* pNPC = (KNpc*)m_FrameData.m_pCharacter;

        bShowLifeBar = pNPC->m_pTemplate->bCanSeeLifeBar;

        nCharacterType = CHARACTER_TYPE_NPC;
    }

    KG_PROCESS_SUCCESS(!g_pRL->m_Option.bHeadSFXEnabled[nCharacterType][CHARACTER_SFX_LIFE - CHARACTER_SFX_BBOX_BALLOON_BEGIN]);

    KG_PROCESS_SUCCESS(!bShowLifeBar);

    pForceRelationModel = GetForceRelationModel(m_RLCharacter.GetObjectID(), g_pRL->m_pSO3World->m_dwClientPlayerID);
    KGLOG_PROCESS_ERROR(pForceRelationModel);

    hr = m_RLCharacter.m_RenderData.SetPercentage(pForceRelationModel->dwColor, m_FrameData.m_Current.fLifePercentage);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
Exit0:
    return;
}

void KRLRemoteCharacter::UpdateAnimationSpeed(RL_ANIMATION_DOWN nAnimationDown, RL_ADVANCE nAdvance)
{
    float fSpeed = 1.0f;

    fSpeed = GetAnimationSpeed(
        m_FrameData.m_Current.nMoveState, 
        m_RLCharacter.m_EquipResource.MDL.fMDLScale, 
        m_FrameData.m_Current.nVelocityXY, 
        m_FrameData.m_Current.bOnRide, 
        nAnimationDown, nAdvance);

    m_RLCharacter.UpdateAnimationSpeed(fSpeed);
}

int KRLRemoteCharacter::UpdateSkillBuff()
{
    int nRetCode = false;
    int nResult = false;
    KSkillBuffParam SkillBuff[SKILL_BUFF_PART_COUNT];

    KGLOG_PROCESS_ERROR(m_FrameData.m_pCharacter);

    m_FrameData.UpdateSkillBuff();

    nRetCode = GetCharacterSkillBuff(m_RLCharacter.m_EquipResource, m_FrameData.m_Current.dwSkillBuffs, SkillBuff);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.UpdateSkillBuff(SkillBuff, _countof(SkillBuff));
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::GetSlopeRotation(D3DXQUATERNION& qRotationLH, RL_TERRAIN_SLOPE nTerrainSlope, BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    float fTerrainYawLH = m_fFootYaw;
    float fTerrainRollLH = 0.0f;
    float fTerrainPitchLH = 0.0f;
    float fPitchStepSpeed = 0.0f;
    float fRollStepSpeed = 0.0f;
    IKG3DScene* p3DScene = NULL;
    D3DXVECTOR3 vPosition;
    BOOL bVisible = FALSE;
    
    bVisible = m_RLCharacter.m_RenderData.IsVisible();

    if (bVisible && !bForceUpdate)
    {
        KG_PROCESS_ERROR(abs(m_fFootYaw - m_fPreviousFootYaw) > FLT_EPSILON || m_bVisible != bVisible);

        m_bVisible = bVisible;
    }

    if (m_FrameData.m_Current.nMoveState == cmsOnJump || 
        m_FrameData.m_Current.nMoveState == cmsOnSwimJump)
    {
        nTerrainSlope = RL_TERRAIN_SLOPE_NONE;
    }

    p3DScene = m_RLCharacter.m_RenderData.GetRLScene()->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    m_RLCharacter.GetPosition(vPosition);

    nRetCode = GetTerrainSlopeLean(p3DScene, vPosition, fTerrainYawLH, nTerrainSlope, fTerrainPitchLH, fTerrainRollLH);
    KG_PROCESS_ERROR(nRetCode);

    fPitchStepSpeed = GetPitchStepSpeed(m_FrameData.m_Current.nMoveState);
    fRollStepSpeed = GetRollStepSpeed(m_FrameData.m_Current.nMoveState);

    if (abs(m_fFootExpectPitch - fTerrainPitchLH) > D3DX_PI / 1800.0f)
        m_fFootExpectPitch = fTerrainPitchLH;
    m_fFootPitch = CloseToPitch(m_fFootExpectPitch, m_fFootPitch, fPitchStepSpeed);

    if (abs(m_fFootExpectRoll - fTerrainRollLH) > D3DX_PI / 1800.0f)
        m_fFootExpectRoll = fTerrainRollLH;
    m_fFootRoll = CloseToRoll(m_fFootExpectRoll, m_fFootRoll, fRollStepSpeed);

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

int KRLRemoteCharacter::PrepareCastSkill(DWORD dwSkillID, DWORD dwSkillLevel)
{
    int nRetCode = false;
    int nResult = false;
    KSkillCasterParam SkillCasterParam;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pCharacter);

    m_FrameData.UpdateCurrentOTAction();
    m_FrameData.UpdateCurrentFightFlag();

    nRetCode = UpdateAnimationUp();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateSheath();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetCharacterSkillCasterParam(m_RLCharacter.m_EquipResource, dwSkillID, dwSkillLevel, &SkillCasterParam);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.PrepareCastSkill(SkillCasterParam);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::CastSkill(DWORD dwSkillID, DWORD dwSkillLevel, DWORD dwBulletID)
{
    int nRetCode = false;
    int nResult = false;
    KSkillCasterParam SkillCasterParam;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pCharacter);

    nRetCode = GetCharacterSkillCasterParam(m_RLCharacter.m_EquipResource, dwSkillID, dwSkillLevel, &SkillCasterParam);
    KG_PROCESS_ERROR(nRetCode);

    m_FrameData.UpdateCurrentOTAction();
    m_FrameData.UpdateCurrentFightFlag();

    nRetCode = UpdateAnimationUp();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateSheath();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_RLCharacter.CastSkill(SkillCasterParam, dwSkillID, dwSkillLevel, dwBulletID);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::UpdateAnimationUp()
{
    int nRetCode = false;
    int nResult = false;
    RL_ANIMATION_UP nAnimationUp = RL_ANIMATION_UP_NONE;

    nAnimationUp = GetRLAnimationUp(m_FrameData.m_Current.nOTAction);

    if (nAnimationUp == RL_ANIMATION_UP_NONE && m_nAnimationUp != RL_ANIMATION_UP_NONE || 
        nAnimationUp != RL_ANIMATION_UP_NONE)
    {
        nRetCode = m_RLCharacter.UpdateAnimationUp(nAnimationUp);
        KGLOG_PROCESS_ERROR(nRetCode);

        m_nAnimationUp = nAnimationUp;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLRemoteCharacter::IsFinished() const
{
    if (m_RLCharacter.IsFinished())
        return true;
    if (m_fExpireTime != 0.0 && g_pRL->m_fTime > m_fExpireTime)
        return true;
    return false;
}

int KRLRemoteCharacter::UpdateSheath()
{
    if (m_bSheath != m_FrameData.m_Current.bSheathFlag && 
        m_FrameData.m_Current.nFightFlag == RL_FIGHT_FLAG_NONE)
    {
        m_bSheath = m_FrameData.m_Current.bSheathFlag;
        m_RLCharacter.UpdateSheath(m_bSheath);
    }

    return true;
}
