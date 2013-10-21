#ifndef CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H
#define CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H

#include "./ktabledefine.h"
#include "../render/krlresource.h"

#define ENABLE_RL_SWIM_Y_ADJUST

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

    BOOL bOnFloor;
};

enum { RL_CHARACTER_POSITION_FRAMEDATA_COUNT = 16 };

class KRLCharacterFrameData
{
private:
	// 表现逻辑关键帧
	struct KRepresentFrameData
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
        BOOL  bAboveWater;
        DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT];

		// Player
        RL_SCHOOL nSchoolID;
        int nJumpCount;
		BOOL bSheathFlag;
		BOOL bUnderWater;
        BOOL bOnRide;

        int nTurn;
    };

public:
	KRLCharacterFrameData();

	void Init(KCharacter* pCharacter);
    int Interpolate(int nVisible, int nForceUpdate);
    int UpdateFrameData(int nGameLoop, int nVisible);
    int InvalidateFrameData(int nGameLoop);
    int UpdateRepresentFrameData(int nVisible);
    int UpdateCurrentRepresentFrameData(int nVisible);
    void UpdateCurrentDirection();
    void UpdateCurrentMoveState();
    void UpdateCurrentOTAction();
    void UpdateSkillBuff();

    D3DXVECTOR3 GetCurrentPosition() const;

    BOOL IsLifeChanged() const;

private:
    int AboveWater(float fWaterHeight, float fFloorHeight, float fCharacterY) const;

    int GetCurrentFrameDataIndex() const;
    int GetNextFrameDataIndex() const;
    int GetFrameDataIndex(int nGameLoop) const;
    void AdvanceFrameDataIndex();

public:
	KCharacter* m_pCharacter;

    KRLCharacterPositionFrameData m_Interpolated;

	KRepresentFrameData m_Previous;
	KRepresentFrameData m_Current;
	
    BOOL m_bOnFloor;

private:
    KRLCharacterPositionFrameData m_FrameData[RL_CHARACTER_POSITION_FRAMEDATA_COUNT];
    int m_nCurrentFrameDataIndex;

    float m_fPositionBeginOffsetY;
    float m_fPositionEndOffsetY;
    float m_fPositionDeltaY;

#ifdef ENABLE_RL_SWIM_Y_ADJUST
    float m_fWaterDelta;  // 3D引擎水面高度与游戏逻辑水面高度差
#endif
};

#endif // CASE_GAME_WORLD_K_RL_CHARACTER_FRAME_DATA_H