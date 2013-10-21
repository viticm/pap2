#ifndef COMMON_K_MATH_TOOLS_H
#define COMMON_K_MATH_TOOLS_H

enum RL_TERRAIN_SLOPE
{
    RL_TERRAIN_SLOPE_NONE = 0x00,
    RL_TERRAIN_SLOPE_LENGTHWAYS = 0x01,  // ◊›œÚPitch
    RL_TERRAIN_SLOPE_TRANSVERSE = 0x02,  // ∫·œÚRoll
    RL_TERRAIN_SLOPE_ALL = RL_TERRAIN_SLOPE_TRANSVERSE | RL_TERRAIN_SLOPE_LENGTHWAYS,
};

class KRLSceneObject;
class IKG3DScene;

KSceneObject* GetSceneObject(DWORD dwObjectType, DWORD dwObject);
void GameWorldCellPointToScenePoint(D3DXVECTOR3& vPosition, int nX, int nY, int nZ, BOOL bAbs);

float ToSceneLength(int nCellLength);
int ToGameWorldLength(float fSceneLength);

float ToSceneVelocity(int nGameWorldVelocity);
int ToGameWorldVelocity(float fSceneVelocity);

float ToSceneAngle(int nGameWorldAngle);
int ToGameWorldAngle(float fSceneAngle);

float ToSceneAngleVelocity(int nGameWorldAngleVelocity);
int ToGameWorldAngleVelocity(float fSceneAngleVelocity);

float YawRHToLH(float fYawRH);
float YawLHToRH(float fYawLH);
float YawLHToModelLH(float fYaw);

float PitchToLH(float fPitchRH);
float PitchToRH(float fPitchLH);

HRESULT GameWorldPositionToScenePosition(
    IKG3DScene* p3DScene,
    D3DXVECTOR3& vPosition,
    float fX, float fY, float fZ,
    BOOL bOnFloor,
    BOOL bOnSlip,
    BOOL bLogicalAdjust,
    int nMaxAltitudeReviseInCM = 0
);

int GetTerrainSlopeLean(IKG3DScene* p3DScene, D3DXVECTOR3 vPosition, float fYawLH, RL_TERRAIN_SLOPE nTerrainSlope, float& fPitchLH, float& fRollLH);
int GetTerrainSlopeRotation(IKG3DScene* p3DScene, D3DXVECTOR3 vPosition, float fYawLH, RL_TERRAIN_SLOPE nTerrainSlope, D3DXQUATERNION& qRotationLH);

void YawToQuatLH(D3DXQUATERNION& q, float fYaw);
void PitchToQuatLH(D3DXQUATERNION& q, float fPitch);
void RollToQuatLH(D3DXQUATERNION& q, float fRoll);

void YawToQuatRH(D3DXQUATERNION& q, float fYaw);
void PitchToQuatRH(D3DXQUATERNION& q, float fPitch);
void RollToQuatRH(D3DXQUATERNION& q, float fRoll);

void VectorToQuatLH(D3DXQUATERNION& qLH, D3DXVECTOR3 const& vDirectionLH);
float VectorToAngleLH(D3DXVECTOR3 const& vDirectionLH);

void GetParallelLine(D3DXVECTOR3& vLine, float fYaw, float fPitch, float fLength);

float GetXZPlanePointDistance(D3DXVECTOR3 const& lhs, D3DXVECTOR3 const& rhs);
float GetXZPlanePointDistanceSqrt(D3DXVECTOR3 const& lhs, D3DXVECTOR3 const& rhs);

int NormalizeDirectionNegativeHalfToPositiveHalf(int nDirection);
int NormalizeDirection(int nDirection);
int SubtractDirection(int nMinuend, int nSubstrahend);
int InterpolateDirection(float* pfResult, int nL, int nR, float fRatioL, float fRatioR);
int CloseToDirection(int nDst, int nSrc, int nStep);

float NormalizeDirectionNegativeHalfToPositiveHalf(float fDirection);
float NormalizeDirection(float fDirection);
float SubtractDirection(float fDst, float fSrc);
float ClampDirection(float fDst, float fSrc, float fClamp);
float CloseToDirection(float fDst, float fSrc, float fStep);
float SubtractDirectionCCW(float fDst, float fSrc);
float SubtractDirectionCW(float fDst, float fSrc);
float ClampDirectionCCW(float fDst, float fSrc, float fClamp);
float ClampDirectionCW(float fDst, float fSrc, float fClamp);
float CloseToDirectionCCW(float fDst, float fSrc, float fStep);
float CloseToDirectionCW(float fDst, float fSrc, float fStep);
float InterpolateDirectionCCW(float fDst, float fSrc, float fRatio);
float InterpolateDirectionCW(float fDst, float fSrc, float fRatio);

float CloseToPitch(float fDst, float fSrc, float fStep);
float CloseToRoll(float fDst, float fSrc, float fStep);

int EqualPosition(const D3DXVECTOR3& crvPoint1, const D3DXVECTOR3& crvPoint2, float fLengthPrecision);
int EqualDirection(const D3DXVECTOR3& crvPoint1, const D3DXVECTOR3& crvPoint2, float fAngelPrecision);

int InterpolatePosition(D3DXVECTOR3& vResult, D3DXVECTOR3 const& vL, D3DXVECTOR3 const& vR, float fRatioL, float fRatioR);

float GetHypotenuseByCosine(float fRightAngleSide, float fAngle);

#endif // COMMON_K_MATH_TOOLS_H