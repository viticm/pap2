#ifndef CASE_GAMEWORLD_KRLLOCALCHARACTERFRAMEDATA_H
#define CASE_GAMEWORLD_KRLLOCALCHARACTERFRAMEDATA_H

#include "./krlcharacterframedata.h"

enum { RL_LOCAL_CHARACTER_POSITION_FRAMEDATA_COUNT = 16 };

class KRLLocalCharacterFrameData : public KRLCharacterFrameData
{
public:
	KRLLocalCharacterFrameData();

    int Interpolate(int nVisible, int nForceUpdate);
    int UpdateFrameData(int nGameLoop, int nVisible);
    int InvalidateFrameData(int nGameLoop);

    D3DXVECTOR3 GetCurrentPosition() const;

private:
    int GetCurrentFrameDataIndex() const;
    int GetNextFrameDataIndex() const;
    int GetFrameDataIndex(int nGameLoop) const;
    void AdvanceFrameDataIndex();

private:
    KRLCharacterPositionFrameData m_FrameData[RL_LOCAL_CHARACTER_POSITION_FRAMEDATA_COUNT];
    int m_nCurrentFrameDataIndex;

    float m_fPositionBeginOffsetY;
    float m_fPositionEndOffsetY;
    float m_fPositionDeltaY;

#ifdef ENABLE_RL_INVALIDATE_FRAMEDATA_ADJUST
    BOOL m_bInvalidateFrameData;
    D3DXVECTOR3 m_vInvalidateAdjustPositionDelta;
    D3DXVECTOR3 m_vInvalidateAdjustPositionDeltaSpeed;
    float m_fInvalidateAdjustDirecitonDelta;
    float m_fInvalidateAdjustDirecitonDeltaSpeed;
#endif
};

#endif // CASE_GAMEWORLD_KRLLOCALCHARACTERFRAMEDATA_H

