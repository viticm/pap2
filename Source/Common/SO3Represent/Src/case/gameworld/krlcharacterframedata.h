#ifndef CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H
#define CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H

#include "./ktabledefine.h"
#include "../render/krlresource.h"
#include "SO3GlobalDef.h"
#include "../../Common/SO3World/Src/KCharacter.h"

#define ENABLE_RL_SWIM_Y_ADJUST
//#define ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST

class KPlayer;


struct KRLCharacterPositionData
{
    int nSubjectiveVelocity;
    int nDirectionXY;
    int nFaceDirection;
    int nX;
    int nY;
    int nZ;
    int nSubjectiveDirection;
    int nJumpCount;
    CHARACTER_MOVE_STATE nMoveState;
    BOOL bSlip;
    BOOL bWater;
    int nCellHighLayer;
};


struct KRLCharacterPositionFrameData
{
    KRLCharacterPositionData PositionData;

    int nGameLoop;

    D3DXVECTOR3 vPosition;

    float fWaterDelta;

    float fFaceDirection;
    float fPitchDirection;

    BOOL bOnFloor;
};

// 表现逻辑关键帧
struct KRLFrameData
{
    // Common
    int nVelocityXY;
    int nDirectionXY;
    int nFaceDirection;
    int nX;
    int nY;
    int nZ;
    CHARACTER_MOVE_STATE nMoveState;
    KOT_ACTION_TYPE nOTAction;
    int nVehicleTrack;
    float fLifePercentage;
    float fFaceDirection;
    float fSubjectiveDirection;
    RL_FIGHT_FLAG nFightFlag;
    BOOL bAboveWater;
    BOOL bSlip;
    DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT];

    // Player
    RL_SCHOOL nSchoolID;
    int nJumpCount;
    BOOL bSheathFlag;
    BOOL bUnderWater;
    BOOL bOnRide;

    int nTurn;
};

class KRLCharacterFrameData
{
public:
	KRLCharacterFrameData();

	void Init(KCharacter* pCharacter);
    int UpdateRepresentFrameData(int nVisible, D3DXVECTOR3 vPosition);
    int UpdateCurrentRepresentFrameData(int nVisible, D3DXVECTOR3 vPosition);
    void UpdateCurrentDirection();
    void UpdateCurrentMoveState();
    void UpdateCurrentOTAction();
    void UpdateCurrentFightFlag();
    void UpdateSkillBuff();

    BOOL IsLifeChanged() const;

    int AboveWater(float fWaterHeight, float fFloorHeight, float fCharacterY) const;

public:
	KCharacter* m_pCharacter;

    KRLCharacterPositionFrameData m_Interpolated;

	KRLFrameData m_Previous;
	KRLFrameData m_Current;

#ifdef ENABLE_RL_SWIM_Y_ADJUST
    float m_fWaterDelta;  // 3D引擎水面高度与游戏逻辑水面高度差
#endif
};

int GetSkillBuffData(KCharacter* pCharacter, DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT]);
int GetWaterHeight(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float& fWaterHeight);
int GetFloorHeight(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float& fFloorHeight);

int GetCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter);
int PrefetchCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter, int nGameLoop);
BOOL OnFloor(KRLCharacterPositionData* pData);

int GetFrameData(KRLCharacterPositionFrameData* pFrameData, KRLCharacterPositionFrameData const* pPrevFrameData, 
                 KCharacter* pCharacter, KRLCharacterPositionData* pPositionData,  int nGameLoop, int nVisible);

int PrefetchEndJumpCharacterPositionData(KRLCharacterPositionData* pData, KCharacter* pCharacter, int* pnGameLoop);

int GetEndJumpOffsetData(KRLCharacterPositionData const* pBeginJumpPositionData, KCharacter* pCharacter, 
                         float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY);

int GetEndDoubleJumpOffsetData(KRLCharacterPositionData const* pBeginJumpPositionData, 
                               float fBeginY, KCharacter* pCharacter, 
                               float* pfBeginOffsetY, float* pfEndOffsetY, float* pfDeltaY);

int AdjustPosition(KCharacter* pCharacter, D3DXVECTOR3 vPosition, float fLengthEpsilon, BOOL* pbAdjust);

#ifdef ENABLE_RL_SWIM_Y_ADJUST
float GetWaterDelta(IKG3DScene* p3DScene, KRLCharacterPositionData* pPositionData, 
                    D3DXVECTOR3 vCharacterPosition, int nCharacterHeight, float fX, float fY, float fZ);
#endif // ENABLE_RL_SWIM_Y_ADJUST

#endif // CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H