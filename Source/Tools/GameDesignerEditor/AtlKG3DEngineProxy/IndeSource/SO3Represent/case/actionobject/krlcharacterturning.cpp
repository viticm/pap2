#include "stdafx.h"
#include "./krlcharacterturning.h"
#include "../../common/kmathtools.h"

static float YAW_EPSILON = D3DX_PI / 180.0f;

static RL_TURNING GetTurningMoving(RL_TURNING nTurning, BOOL bMoving)
{
    if (bMoving)
    {
        switch (nTurning)
        {
        case RL_TURNING_STAND_NONE:
            nTurning = RL_TURNING_MOVING_NONE;
            break;
        case RL_TURNING_STAND_FACE_TURN_LEFT:
            nTurning = RL_TURNING_MOVING_FACE_TURN_LEFT;
            break;
        case RL_TURNING_STAND_FACE_TURN_RIGHT:
            nTurning = RL_TURNING_MOVING_FACE_TURN_RIGHT;
            break;
        case RL_TURNING_STAND_FOOT_TURN_LEFT:
        case RL_TURNING_STAND_FOOT_TURN_RIGHT:
            nTurning = RL_TURNING_MOVING_NONE;
            break;
        case RL_TURNING_MOVING_NONE:
        case RL_TURNING_MOVING_FACE_TURN_LEFT:
        case RL_TURNING_MOVING_FACE_TURN_RIGHT:
            break;
        default:
            ASSERT(0);
            break;
        }
    }
    else
    {
        switch (nTurning)
        {
        case RL_TURNING_STAND_NONE:
        case RL_TURNING_STAND_FACE_TURN_LEFT:
        case RL_TURNING_STAND_FACE_TURN_RIGHT:
        case RL_TURNING_STAND_FOOT_TURN_LEFT:
        case RL_TURNING_STAND_FOOT_TURN_RIGHT:
            break;
        case RL_TURNING_MOVING_NONE:
            nTurning = RL_TURNING_STAND_NONE;
            break;
        case RL_TURNING_MOVING_FACE_TURN_LEFT:
            nTurning = RL_TURNING_STAND_FACE_TURN_LEFT;
            break;
        case RL_TURNING_MOVING_FACE_TURN_RIGHT:
            nTurning = RL_TURNING_STAND_FACE_TURN_RIGHT;
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    return nTurning;
}

static RL_TURNING GetTurningStandFootDirection(float fFaceFootDeltaYaw, float fPreviousFaceFootDeltaYaw)
{
    if (fFaceFootDeltaYaw > 0.0f)
        fFaceFootDeltaYaw += YAW_EPSILON;
    else
        fFaceFootDeltaYaw -= YAW_EPSILON;

    float fDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceFootDeltaYaw - fPreviousFaceFootDeltaYaw);

    if (abs(fDeltaYaw) <= YAW_EPSILON)
        return RL_TURNING_STAND_NONE;

    if (fDeltaYaw < -YAW_EPSILON)
        return RL_TURNING_STAND_FOOT_TURN_LEFT;

    return RL_TURNING_STAND_FOOT_TURN_RIGHT;
}

static RL_TURNING GetTurningStandFaceDirection(float fFaceYaw, float fPreviousFaceYaw)
{
    float fDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fPreviousFaceYaw);

    if (abs(fDeltaYaw) <= YAW_EPSILON)
        return RL_TURNING_STAND_NONE;

    if (fDeltaYaw < -YAW_EPSILON)
        return RL_TURNING_STAND_FACE_TURN_LEFT;

    return RL_TURNING_STAND_FACE_TURN_RIGHT;
}

static RL_TURNING GetTurningStandNone(RL_TURNING nTurning, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    float fFaceFootDeltaYaw = 0.0f;

    ASSERT(nTurning == RL_TURNING_STAND_NONE);

    fFaceFootDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fFootYaw);

    if (abs(fFaceFootDeltaYaw) >= D3DX_PI / 2.01f)
    {
        // ÏòÁ³³¯ÏòÅ²½Å

        float fPreviousFaceFootDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fPreviousFaceYaw - fPreviousFootYaw);

        nTurning = GetTurningStandFootDirection(fFaceFootDeltaYaw, fPreviousFaceFootDeltaYaw);
    }
    else
    {
        if (abs(fFaceFootDeltaYaw) > YAW_EPSILON)
        {
            // ×ªÉí¡¢Å²½Å

            nTurning = GetTurningStandFaceDirection(fFaceYaw, fPreviousFaceYaw);
            if (nTurning == RL_TURNING_STAND_NONE)
            {
                float fPreviousFaceFootDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fPreviousFaceYaw - fPreviousFootYaw);

                nTurning = GetTurningStandFootDirection(fPreviousFaceFootDeltaYaw, fFaceFootDeltaYaw);
            }
        }
    }

    return nTurning;
}

static RL_TURNING GetTurningStandFace(RL_TURNING nTurning, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    RL_TURNING nNextTurning = RL_TURNING_STAND_NONE;
    float fFaceFootDeltaYaw = 0.0f;
    
    ASSERT(nTurning == RL_TURNING_STAND_FACE_TURN_LEFT || nTurning == RL_TURNING_STAND_FACE_TURN_RIGHT);

    fFaceFootDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fFootYaw);

    nNextTurning = GetTurningStandFaceDirection(fFaceYaw, fPreviousFaceYaw);

    if (abs(fFaceFootDeltaYaw) >= D3DX_PI / 2.01f)
    {
        switch (nNextTurning)
        {
        case RL_TURNING_STAND_NONE:
            switch (nTurning)
            {
            case RL_TURNING_STAND_FACE_TURN_LEFT:
                nNextTurning = RL_TURNING_STAND_FOOT_TURN_LEFT;
                break;
            case RL_TURNING_STAND_FACE_TURN_RIGHT:
                nNextTurning = RL_TURNING_STAND_FOOT_TURN_RIGHT;
                break;
            default:
                ASSERT(0);
                break;
            }
            break;
        case RL_TURNING_STAND_FACE_TURN_LEFT:
            nNextTurning = RL_TURNING_STAND_FOOT_TURN_LEFT;
            break;
        case RL_TURNING_STAND_FACE_TURN_RIGHT:
            nNextTurning = RL_TURNING_STAND_FOOT_TURN_RIGHT;
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    return nNextTurning;
}

static RL_TURNING GetTurningStandFoot(RL_TURNING nTurning, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    RL_TURNING nNextTurning = RL_TURNING_STAND_NONE;
    RL_TURNING nFaceTurning = RL_TURNING_STAND_NONE;
    float fFaceFootDeltaYaw = 0.0f;

    ASSERT(nTurning == RL_TURNING_STAND_FOOT_TURN_LEFT || nTurning == RL_TURNING_STAND_FOOT_TURN_RIGHT);

    fFaceFootDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fFootYaw);

    if (abs(fFaceFootDeltaYaw) <= YAW_EPSILON)
    {
        // Å²½ÅÍ£Ö¹

        nNextTurning = RL_TURNING_STAND_NONE;
    }
    else
    {
        // ×ªÏò

        nFaceTurning = GetTurningStandFaceDirection(fFaceYaw, fPreviousFaceYaw);
        switch (nTurning)
        {
        case RL_TURNING_STAND_FOOT_TURN_LEFT:
            if (nFaceTurning == RL_TURNING_STAND_FACE_TURN_RIGHT)
                nNextTurning = RL_TURNING_STAND_NONE;
            else
                nNextTurning = nTurning;
            break;
        case RL_TURNING_STAND_FOOT_TURN_RIGHT:
             if (nFaceTurning == RL_TURNING_STAND_FACE_TURN_LEFT)
                nNextTurning = RL_TURNING_STAND_NONE;
            else
                nNextTurning = nTurning;
            break;
        default:
            ASSERT(0);
            break;
        }
    }

    return nNextTurning;
}

static RL_TURNING GetTurningMovingFaceDirection(float fFaceYaw, float fPreviousFaceYaw)
{
    float fDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fFaceYaw - fPreviousFaceYaw);

    if (abs(fDeltaYaw) <= YAW_EPSILON)
        return RL_TURNING_MOVING_NONE;

    if (fDeltaYaw < -YAW_EPSILON)
        return RL_TURNING_MOVING_FACE_TURN_LEFT;

    return RL_TURNING_MOVING_FACE_TURN_RIGHT;
}

static RL_TURNING GetTurningMovingNone(RL_TURNING nTurning, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    ASSERT(nTurning == RL_TURNING_MOVING_NONE);

    nTurning = GetTurningMovingFaceDirection(fFaceYaw, fPreviousFaceYaw);

    return nTurning;
}

static RL_TURNING GetTurningMovingFace(RL_TURNING nTurning, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    RL_TURNING nNextTurning = RL_TURNING_MOVING_NONE;
    float fFaceDeltaYaw = 0.0f;

    ASSERT(nTurning == RL_TURNING_MOVING_FACE_TURN_LEFT || nTurning == RL_TURNING_MOVING_FACE_TURN_RIGHT);

    fFaceDeltaYaw = NormalizeDirectionNegativeHalfToPositiveHalf(fPreviousFaceYaw - fFaceYaw);

    if (abs(fFaceDeltaYaw) > YAW_EPSILON)
        nNextTurning = nTurning;

    return nNextTurning;
}

RL_TURNING GetTurning(RL_TURNING nTurning, BOOL bMoving, float fFaceYaw, float fFootYaw, float fPreviousFaceYaw, float fPreviousFootYaw)
{
    nTurning = GetTurningMoving(nTurning, bMoving);

    switch (nTurning)
    {
    case RL_TURNING_STAND_NONE:
        nTurning = GetTurningStandNone(nTurning, fFaceYaw, fFootYaw, fPreviousFaceYaw, fPreviousFootYaw);
        break;
    case RL_TURNING_STAND_FACE_TURN_LEFT:
    case RL_TURNING_STAND_FACE_TURN_RIGHT:
        nTurning = GetTurningStandFace(nTurning, fFaceYaw, fFootYaw, fPreviousFaceYaw, fPreviousFootYaw);
        break;
    case RL_TURNING_STAND_FOOT_TURN_LEFT:
    case RL_TURNING_STAND_FOOT_TURN_RIGHT:
        nTurning = GetTurningStandFoot(nTurning, fFaceYaw, fFootYaw, fPreviousFaceYaw, fPreviousFootYaw);
        break;
    case RL_TURNING_MOVING_NONE:
        nTurning = GetTurningMovingNone(nTurning, fFaceYaw, fFootYaw, fPreviousFaceYaw, fPreviousFootYaw);
        break;
    case RL_TURNING_MOVING_FACE_TURN_LEFT:
    case RL_TURNING_MOVING_FACE_TURN_RIGHT:
        nTurning = GetTurningMovingFace(nTurning, fFaceYaw, fFootYaw, fPreviousFaceYaw, fPreviousFootYaw);
        break;
    default:
        ASSERT(0);
        break;
    }

    return nTurning;
}
