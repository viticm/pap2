#include "stdafx.h"
#include "./kmathtools.h"
#include "../../Source/Common/SO3World/Src/KPlayer.h"
#include "../../Source/Common/SO3World/Src/KNpc.h"
#include "../../Source/Common/SO3World/Src/KDoodad.h"
#include "../case/gameworld/ktablelist.h"
#include "../SO3Represent.h"
#include "KG3DEngine/KG3DInterface.h"

namespace
{
	D3DXVECTOR3 const AXIS_PITCH_LH = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 const AXIS_YAW_LH = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 const AXIS_ROLL_LH = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXVECTOR3 const AXIS_PITCH_RH = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 const AXIS_YAW_RH = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DXVECTOR3 const AXIS_ROLL_RH = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
}

KSceneObject* GetSceneObject(DWORD dwObjectType, DWORD dwObject)
{
    KSceneObject* pObject = NULL;

    switch (dwObjectType)
    {
    case RL_OBJECT_PLAYER:
    case RL_OBJECT_RIDES:
        pObject = GetPlayer(dwObject);
        break;
    case RL_OBJECT_NPC:
        pObject = GetNpc(dwObject);	
        break;
    case RL_OBJECT_DOODAD:
        pObject = GetDoodad(dwObject);
        break;
    case RL_OBJECT_UNKNOWN:	/* fall through */
    default:
        break;
    }
    return pObject;
}

void GameWorldCellPointToScenePoint(D3DXVECTOR3& vPosition, int nX, int nY, int nZ, BOOL bAbs)
{
    vPosition.x = ToSceneLength(nX);
    vPosition.z = ToSceneLength(nY);
    if (bAbs) //绝对坐标Z轴转化
        vPosition.y = (float)nZ / POINT_PER_ALTITUDE * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;
    else
        vPosition.y = (float)nZ / POINT_PER_ALTITUDE * ALTITUDE_UNIT;
}

int ToGameWorldLength(float fSceneLength)
{
    return (int)(fSceneLength * CELL_LENGTH / LOGICAL_CELL_CM_LENGTH);
}

float ToSceneLength(int nCellLength)
{
    return (float)(nCellLength) * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH;
}

int ToGameWorldVelocity(float fSceneVelocity)
{
    return ToGameWorldLength(fSceneVelocity * 1000.0f / GAME_FPS);
}

float ToSceneVelocity(int nGameWorldVelocity)
{
    return ToSceneLength(nGameWorldVelocity) * GAME_FPS / 1000.0f;
}

HRESULT GameWorldPositionToScenePosition(
    IKG3DScene* p3DScene, D3DXVECTOR3& vPosition, float fX, float fY, float fZ, 
    BOOL bOnFloor, BOOL bOnSlip, BOOL bLogicalAdjust, int nMaxAltitudeReviseInCM)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(p3DScene);

    if (bOnFloor)
    {
        D3DXVECTOR3 vGameWorldPosition;
        float fOffsetY = 0.0f;

        hr = p3DScene->LogicalToScene(&vPosition, fX, fY, fZ, TRUE);
        KGLOG_COM_PROCESS_ERROR(hr);
        
        if (bLogicalAdjust)
        {
            hr = p3DScene->LogicalToScene(&vGameWorldPosition, fX, fY, fZ, FALSE);
            KGLOG_COM_PROCESS_ERROR(hr);

            fOffsetY = vGameWorldPosition.y - vPosition.y;

            if (fOffsetY > 64.0f && !bOnSlip || fOffsetY > 200.0f)
            {
                vPosition.y = vGameWorldPosition.y /*  - ALTITUDE_UNIT * 20 */;
            }
        }
    }
    else
    {
        hr = p3DScene->LogicalToScene(&vPosition, fX, fY, fZ, FALSE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int GetTerrainSlopeLean(IKG3DScene* p3DScene, D3DXVECTOR3 vPosition, float fYawLH, RL_TERRAIN_SLOPE nTerrainSlope, float& fPitchLH, float& fRollLH)
{
    static float const RADIUS = 0.1f;

    int nResult = false;
    HRESULT hr = E_FAIL;
    float fYawRH = 0.0f;
    float fPitchHeight = 0.0f;
    float fRollHeight = 0.0f;
    float yCenterFloor = 0.0f;
    float yBackFloor = 0.0f;
    float yLeftFloor = 0.0f;
    D3DXVECTOR3 vCenter;
    D3DXVECTOR3 vBack;
    D3DXVECTOR3 vLeft;

    fPitchLH = 0.0f;
    fRollLH = 0.0f;

    KG_PROCESS_SUCCESS(g_pRL->m_CurrentTerrainLeanOption.bDisable);

    KGLOG_PROCESS_ERROR(p3DScene);

 	KG_PROCESS_SUCCESS(nTerrainSlope == RL_TERRAIN_SLOPE_NONE);

    fYawRH = YawLHToRH(fYawLH);

    vCenter = vPosition;
    vCenter.y += 50.0f;

    hr = p3DScene->GetFloor(&vCenter, &yCenterFloor);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (yCenterFloor > vPosition.y)
    {
        // 说明在地面50厘米间有物件

        vCenter.y = vPosition.y;
        yCenterFloor = vPosition.y;
    }

    if (nTerrainSlope & RL_TERRAIN_SLOPE_LENGTHWAYS)
    {
        vBack.x = vCenter.x - RADIUS * cos(fYawRH) ;
        vBack.y = vCenter.y;
        vBack.z = vCenter.z - RADIUS * sin(fYawRH);

        hr = p3DScene->GetFloor(&vBack, &yBackFloor);
        KGLOG_COM_PROCESS_ERROR(hr);

        fPitchHeight = yCenterFloor - yBackFloor;

        if (abs(fPitchHeight) > 3.0f)
        {
            fPitchHeight = 0.0f;
        }

        fPitchLH = atan2f(fPitchHeight, RADIUS);
    }

    if (nTerrainSlope & RL_TERRAIN_SLOPE_TRANSVERSE)
    {
        vLeft.x = vCenter.x + RADIUS * sin(fYawRH);
        vLeft.y = vCenter.y;
        vLeft.z = vCenter.z - RADIUS * cos(fYawRH);

        hr = p3DScene->GetFloor(&vLeft, &yLeftFloor);
        KGLOG_COM_PROCESS_ERROR(hr);

        fRollHeight = yCenterFloor - yLeftFloor;

        if (abs(fRollHeight) > 3.0f)
        {
            fRollHeight = 0.0f;
        }

        fRollLH = atan2f(fRollHeight, RADIUS);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int GetTerrainSlopeRotation(IKG3DScene* p3DScene, D3DXVECTOR3 vPosition, float fYawLH, RL_TERRAIN_SLOPE nTerrainSlope, D3DXQUATERNION& qRotationLH)
{
    int nRetCode = false;
    int nResult = false;
    float fTerrainYawLH = fYawLH;
    float fTerrainRollLH = 0.0f;
    float fTerrainPitchLH = 0.0f;

    switch (nTerrainSlope)
    {
    case RL_TERRAIN_SLOPE_NONE:
        YawToQuatLH(qRotationLH, YawLHToModelLH(fYawLH));
        break;
    case RL_TERRAIN_SLOPE_LENGTHWAYS:
        nRetCode = GetTerrainSlopeLean(p3DScene, vPosition, fTerrainYawLH, nTerrainSlope, fTerrainPitchLH, fTerrainRollLH);
        KG_PROCESS_ERROR(nRetCode);

        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), fTerrainPitchLH, 0.0f);
        break;
    case RL_TERRAIN_SLOPE_TRANSVERSE:
        nRetCode = GetTerrainSlopeLean(p3DScene, vPosition, fTerrainYawLH, nTerrainSlope, fTerrainPitchLH, fTerrainRollLH);
        KG_PROCESS_ERROR(nRetCode);

        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), 0.0f, fTerrainRollLH);
        break;
    case RL_TERRAIN_SLOPE_ALL:
        nRetCode = GetTerrainSlopeLean(p3DScene, vPosition, fTerrainYawLH, nTerrainSlope, fTerrainPitchLH, fTerrainRollLH);
        KG_PROCESS_ERROR(nRetCode);

        D3DXQuaternionRotationYawPitchRoll(&qRotationLH, YawLHToModelLH(fTerrainYawLH), fTerrainPitchLH, fTerrainRollLH);
        break;
    default:
        ASSERT(0);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

float ToSceneAngle(int nGameWorldAngle)
{
    while (nGameWorldAngle >= DIRECTION_COUNT)
        nGameWorldAngle -= DIRECTION_COUNT;
    while (nGameWorldAngle < 0)
        nGameWorldAngle += DIRECTION_COUNT;
    return D3DX_PI * 2.0f / (float)DIRECTION_COUNT * (float)nGameWorldAngle;
}

int ToGameWorldAngle(float fSceneAngle)
{
    int nGameWorldAngle = 0;

    float fBias = (2.0f * D3DX_PI) / (float)DIRECTION_COUNT / 2.0f;

    while (fSceneAngle < 0.0f)
        fSceneAngle += 2.0f * D3DX_PI;
    while (fSceneAngle >= 2.0f * D3DX_PI)
        fSceneAngle -= 2.0f * D3DX_PI;
    nGameWorldAngle = (int)((float)DIRECTION_COUNT / (2.0f * D3DX_PI) * (fSceneAngle + fBias)) % DIRECTION_COUNT;

    ASSERT(nGameWorldAngle >= 0 && nGameWorldAngle < DIRECTION_COUNT);
    return nGameWorldAngle;
}

float ToSceneAngleVelocity(int nGameWorldAngleVelocity)
{
    return ToSceneAngle(nGameWorldAngleVelocity) * GAME_FPS / 1000.0f;
}

int ToGameWorldAngleVelocity(float fSceneAngleVelocity)
{
    return ToGameWorldAngle(fSceneAngleVelocity * 1000.0f / GAME_FPS);
}

float YawRHToLH(float fYawRH)
{
    return D3DX_PI / 2.0f - fYawRH;
}

float YawLHToRH(float fYawLH)
{
    return D3DX_PI / 2.0f - fYawLH;
}

float YawLHToModelLH(float fYaw)
{
    return fYaw + D3DX_PI;
}

float PitchToLH(float fPitchRH)
{
    return fPitchRH;
}

float PitchToRH(float fPitchLH)
{
    return fPitchLH;
}

void YawToQuatLH(D3DXQUATERNION& q, float fYaw)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_YAW_LH, fYaw);
}

void PitchToQuatLH(D3DXQUATERNION& q, float fPitch)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_PITCH_LH, fPitch);
}

void RollToQuatLH(D3DXQUATERNION& q, float fRoll)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_ROLL_LH, fRoll);
}

void YawToQuatRH(D3DXQUATERNION& q, float fYaw)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_YAW_RH, fYaw);
}

void PitchToQuatRH(D3DXQUATERNION& q, float fPitch)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_PITCH_RH, fPitch);
}

void RollToQuatRH(D3DXQUATERNION& q, float fRoll)
{
	D3DXQuaternionRotationAxis(&q, &AXIS_ROLL_RH, fRoll);
}

void VectorToQuatLH(D3DXQUATERNION& qLH, D3DXVECTOR3 const& vLH)
{
	// TODO: 目前忽略y-axis

	float fAngleLH = VectorToAngleLH(vLH);

	YawToQuatLH(qLH, fAngleLH);
}

float VectorToAngleLH(D3DXVECTOR3 const& vLH)
{
	return atan2f(vLH.x, vLH.z);
}

void GetParallelLine(D3DXVECTOR3& vLine, float fYaw, float fPitch, float fLength)
{
    D3DXMATRIX mRotation;

    D3DXMatrixRotationY(&mRotation, fYaw);
    vLine = D3DXVECTOR3(0.0f, fLength * sinf(fPitch), fLength * cosf(fPitch));
    D3DXVec3TransformCoord(&vLine, &vLine, &mRotation);
}

float GetXZPlanePointDistance(D3DXVECTOR3 const& lhs, D3DXVECTOR3 const& rhs)
{
	float fRadiusSqrt = GetXZPlanePointDistanceSqrt(lhs, rhs);
	return sqrtf(fRadiusSqrt);
}

float GetXZPlanePointDistanceSqrt(D3DXVECTOR3 const& lhs, D3DXVECTOR3 const& rhs)
{
	float fDeltaX = rhs.x - lhs.x;
	float fDeltaZ = rhs.z - lhs.z;

	return fDeltaX * fDeltaX + fDeltaZ * fDeltaZ;
}

int NormalizeDirectionNegativeHalfToPositiveHalf(int nDirection)
{
    while (nDirection >= DIRECTION_COUNT / 2)
        nDirection -= DIRECTION_COUNT;
    while (nDirection < -DIRECTION_COUNT / 2)
        nDirection += DIRECTION_COUNT;
	return nDirection;
}

int NormalizeDirection(int nDirection)
{
	while (nDirection >= DIRECTION_COUNT)
		nDirection -= DIRECTION_COUNT;
	while (nDirection < 0)
		nDirection += DIRECTION_COUNT;
	return nDirection;
}

int InterpolateDirection(float* pfResult, int nL, int nR, float fRatioL, float fRatioR)
{
    int nResult = false;
    int nDelta = 0;
    float fRange = 0.0f;
    float fResult = 0.0f;

    KGLOG_PROCESS_ERROR(pfResult);

    fRange = fRatioL + fRatioR;
    KG_PROCESS_ERROR(fRange > FLT_EPSILON);

    nDelta = SubtractDirection(nR, nL);
    if (nDelta != 0)
    {
        fResult = (float)nL + (float)nDelta * (fRatioL / fRange);

        while (fResult >= (float)DIRECTION_COUNT)
            fResult -= (float)DIRECTION_COUNT;
        while (fResult < 0.0f)
            fResult += (float)DIRECTION_COUNT;
    }
    else
    {
        fResult = (float)nL;
    }

    *pfResult = fResult;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pfResult)
        {
            *pfResult = (float)nL;
        }
    }
    return nResult;
}

int SubtractDirection(int nMinuend, int nSubstrahend)
{
    nMinuend = NormalizeDirectionNegativeHalfToPositiveHalf(nMinuend);
    nSubstrahend = NormalizeDirectionNegativeHalfToPositiveHalf(nSubstrahend);

    return NormalizeDirectionNegativeHalfToPositiveHalf(nMinuend - nSubstrahend);
}

float SubtractDirection(float fMinuend, float fSubstrahend)
{
    fMinuend = NormalizeDirectionNegativeHalfToPositiveHalf(fMinuend);
    fSubstrahend = NormalizeDirectionNegativeHalfToPositiveHalf(fSubstrahend);

    return NormalizeDirectionNegativeHalfToPositiveHalf(fMinuend - fSubstrahend);
}

int CloseToDirection(int nDst, int nSrc, int nStep)
{
    int nDelta = SubtractDirection(nDst, nSrc);

    if (abs(nDelta) < nStep)
        return nDst;

    if (nDelta > 0)
        return nSrc - nStep;

    return nSrc + nStep;
}

float ClampDirection(float fDst, float fSrc, float fClamp)
{
    if (abs(fDst - fSrc) < FLT_EPSILON)
        return fDst;
    if (abs(SubtractDirectionCCW(fDst, fSrc)) < abs(SubtractDirectionCW(fDst, fSrc)))
        return ClampDirectionCCW(fDst, fSrc, fClamp);
    return ClampDirectionCW(fDst, fSrc, fClamp);
}

float CloseToDirection(float fDst, float fSrc, float fStep)
{
    if (abs(fDst - fSrc) < FLT_EPSILON)
        return fDst;
    if (abs(SubtractDirectionCCW(fDst, fSrc)) < abs(SubtractDirectionCW(fDst, fSrc)))
        return CloseToDirectionCCW(fDst, fSrc, fStep);
    return CloseToDirectionCW(fDst, fSrc, fStep);
}

float NormalizeDirectionNegativeHalfToPositiveHalf(float fDirection)
{
    while (fDirection >= D3DX_PI)
        fDirection -= D3DX_PI * 2.0f;
    while (fDirection < -D3DX_PI)
        fDirection += D3DX_PI * 2.0f;
	return fDirection;
}

float NormalizeDirection(float fDirection)
{
	while (fDirection >= D3DX_PI * 2.0f)
		fDirection -= D3DX_PI * 2.0f;
	while (fDirection < 0.0f)
		fDirection += D3DX_PI * 2.0f;
	return fDirection;
}

float SubtractDirectionCCW(float fDst, float fSrc)
{
    fSrc = NormalizeDirection(fSrc);
    fDst = NormalizeDirection(fDst);

    float fDelta = fDst - fSrc;

    if (fDelta < 0.0f)
        fDelta += D3DX_PI * 2.0f;

    return fDelta;
}

float SubtractDirectionCW(float fDst, float fSrc)
{
    fSrc = NormalizeDirection(fSrc);
    fDst = NormalizeDirection(fDst);

    float fDelta = fDst - fSrc;

    if (fDelta > 0.0f)
        fDelta -= D3DX_PI * 2.0f;

    return fDelta;
}

// Desc: 
// if abs(fSrc - fDst) < fClamp, return fSrc, else return fDst - fClamp
float ClampDirectionCCW(float fDst, float fSrc, float fClamp)
{
    float fResult = fSrc;

    float fDelta = SubtractDirectionCCW(fDst, fSrc);

    if (fDelta > fClamp)
        fResult = fDst - fClamp;

    fResult = NormalizeDirection(fResult);
    
    return fResult;
}

// Desc: 
// if abs(fSrc - fDst) < fClamp, return fSrc, else return fDst - fClamp
float ClampDirectionCW(float fDst, float fSrc, float fClamp)
{
    float fResult = fSrc;

    float fDelta = SubtractDirectionCW(fDst, fSrc);

    if (fDelta < -fClamp)
        fResult = fDst + fClamp;

    fResult = NormalizeDirection(fResult);
    
    return fResult;
}

float CloseToDirectionCCW(float fDst, float fSrc, float fStep)
{
    float fResult = fDst;

    float fDelta = SubtractDirectionCCW(fDst, fSrc);
    if (abs(fDelta) > FLT_EPSILON)
    {
        if (fDelta > fStep)
            fResult = fSrc + fStep;

        fResult = NormalizeDirection(fResult);
    }

    return fResult;
}

float CloseToDirectionCW(float fDst, float fSrc, float fStep)
{
    float fResult = fDst;

    float fDelta = SubtractDirectionCW(fDst, fSrc);
    if (abs(fDelta) > FLT_EPSILON)
    {
        if (fDelta < -fStep)
            fResult = fSrc - fStep;

        fResult = NormalizeDirection(fResult);
    }

    return fResult;
}

float InterpolateDirectionCCW(float fDst, float fSrc, float fRatio)
{
    float fDelta = SubtractDirectionCCW(fDst, fSrc);
    float fResult = NormalizeDirection(fDelta * fRatio + fSrc);
    return fResult;
}

float InterpolateDirectionCW(float fDst, float fSrc, float fRatio)
{
    float fDelta = SubtractDirectionCW(fDst, fSrc);
    float fResult = NormalizeDirection(fDelta * fRatio + fSrc);
    return fResult;
}

float CloseToPitch(float fDst, float fSrc, float fStep)
{
    ASSERT(fStep > 0.0f);

    float fDelta = fDst - fSrc;
    if (fDelta > fStep)
        return fSrc + fStep;
    if (fDelta < -fStep)
        return fSrc - fStep;
    return fDst;
}

float CloseToRoll(float fDst, float fSrc, float fStep)
{
    ASSERT(fStep > 0.0f);

    float fDelta = fDst - fSrc;
    if (fDelta > fStep)
        return fSrc + fStep;
    if (fDelta < -fStep)
        return fSrc - fStep;
    return fDst;
}

// 粗略地判断两个点是否在同一位置
int EqualPosition(const D3DXVECTOR3& crvPoint1, const D3DXVECTOR3& crvPoint2, float fLengthPrecision = 0.5f)
{
    float fLength = D3DXVec3Length(&(crvPoint2 - crvPoint1));
    return (fLength <= fLengthPrecision);
}

// 粗略地两个方向向量是否是同一方向
int EqualDirection(const D3DXVECTOR3& crvPoint1, const D3DXVECTOR3& crvPoint2, float fAngelPrecision = D3DX_PI / 180)
{
    D3DXVECTOR3 vNormPoint1(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vNormPoint2(0.0f, 0.0f, 0.0f);
    float fDot = 0.0f;
    float fAngle = 0.0f;

    D3DXVec3Normalize(&vNormPoint1, &crvPoint1);
    D3DXVec3Normalize(&vNormPoint2, &crvPoint2);
    fDot = D3DXVec3Dot(&vNormPoint1, &vNormPoint2);
    if (fDot >  1.0f)
        fDot =  1.0f;
    if (fDot < -1.0f)
        fDot = -1.0f;
    fAngle = acosf(fDot);

    return (fAngle <= fAngelPrecision);
}


int InterpolatePosition(D3DXVECTOR3& vResult, D3DXVECTOR3 const& vL, D3DXVECTOR3 const& vR, float fRatioL, float fRatioR)
{
    int nRetCode = false;
    int nResult = false;
    float fRange = 0.0f;
    float fRangeReciprocal = 0.0f;
    D3DXVECTOR3 vDelta;

    KG_PROCESS_ERROR(fRatioL >= 0.0f);
    KG_PROCESS_ERROR(fRatioR >= 0.0f);

    fRange = fRatioL + fRatioR;
    vDelta = vR - vL;

	nRetCode = D3DXVec3Length(&vDelta) > 1.0f;
    KG_PROCESS_ERROR(nRetCode);
	
    nRetCode = abs(fRange) > FLT_EPSILON;
	KG_PROCESS_ERROR(nRetCode);
	
    fRangeReciprocal = 1.0f / fRange;

    fRatioL *= fRangeReciprocal;
    fRatioR *= fRangeReciprocal;

    vResult = vL * fRatioR + vR * fRatioL;

    nResult = true;
Exit0:
    if (!nResult)
    {
        vResult = vL;
    }
    return nResult;
}

float GetHypotenuseByCosine(float fRightAngleSide, float fAngle)
{
    float fHypotenuse = 0.0f;
    float fCosine = cosf(fAngle);

    KG_ASSERT_EXIT(abs(fCosine) > FLT_EPSILON);

    fHypotenuse = fRightAngleSide / fCosine;
Exit0:
    return fHypotenuse;
}

static double s_fMilliSecondsPerTick = 0.0;
static double s_fTickCount = 0.0;
static LARGE_INTEGER s_liFirstCounter = { 0 };

int InitPerformanceTickCount()
{
    int nRetCode = false;
    int nResult = false;
    LARGE_INTEGER lv;
    HANDLE hThread = NULL;
    DWORD_PTR dwMask = 0;

    hThread = GetCurrentThread();

    dwMask = SetThreadAffinityMask(hThread, 0x00000001);

    nRetCode = QueryPerformanceFrequency(&lv);
    KGLOG_PROCESS_ERROR(nRetCode);

    s_fMilliSecondsPerTick = 1000.0 / lv.QuadPart;

    nRetCode = QueryPerformanceCounter(&s_liFirstCounter);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

double GetPerformanceTickCount()
{
    int nRetCode = false;
    double fTickCount = 0.0;

    LARGE_INTEGER lv;

    nRetCode = QueryPerformanceCounter(&lv);
    KGLOG_PROCESS_ERROR(nRetCode);

    fTickCount = s_fMilliSecondsPerTick * (lv.QuadPart - s_liFirstCounter.QuadPart);

    s_fTickCount = fTickCount;

Exit0:
    return fTickCount;
}

