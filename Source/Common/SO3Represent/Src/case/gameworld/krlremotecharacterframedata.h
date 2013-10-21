#ifndef CASE_GAMEWORLD_KRREMOTECHARACTERFRAMEDATA_H
#define CASE_GAMEWORLD_KRREMOTECHARACTERFRAMEDATA_H

#include "./krlcharacterframedata.h"

struct KRLRemoteCharacterPositionData
{
    double fFrameTime;
    D3DXVECTOR3 vPosition;

    float fFaceDirection;
    float fPitchDirection;
};

enum { RL_REMOTE_CHARACTER_POSITION_FRAMEDATA_COUNT = 16 };

class KRLRemoteCharacterFrameData : public KRLCharacterFrameData
{
public:
    KRLRemoteCharacterFrameData();

    int Interpolate(int nVisible, int nForceUpdate);
    int UpdateFrameData(int nGameLoop, int nVisible);

    int UpdatePositionOffset();

    D3DXVECTOR3 GetCurrentPosition() const;

private:
    KRLRemoteCharacterPositionData m_FrameData[RL_REMOTE_CHARACTER_POSITION_FRAMEDATA_COUNT];

    int m_nLastRecord;
    int m_nRecordCount;

    int m_nX;
    int m_nY;
    int m_nZ;

    BOOL m_bOnFloor;

    float m_fPositionOffsetY;
};

#endif // CASE_GAMEWORLD_KRREMOTECHARACTERFRAMEDATA_H

