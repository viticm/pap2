#ifndef KRLTRACECHARACTER_H
#define KRLTRACECHARACTER_H

struct KRLTraceCharacter
{
    D3DXVECTOR3 vPosition;
    D3DXVECTOR3 vChangedPosition;
    BOOL bPositionChanged;
    float fChangedLength;

    double fChangedTime;
    double fChangedPeriod;

    float fFaceDirection;
    float fPitchDirection;
};

int TraceCharacter(KRLTraceCharacter* pRLTraceCharacter, D3DXVECTOR3 vPosition, float fFaceDirection, float fPitchDirection);

#endif // KRLTRACECHARACTER_H
