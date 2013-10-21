////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIVM.h
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : AI 状态机外壳
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KAIVM_H_ 
#define _KAIVM_H_ 

#include "KAIState.h"
#include "KAIAction.h"

class KAIState;
class KCharacter;
class KAILogic;

enum KAI_EVENT
{
    aevInvalid  = 0,

    aevOnPrimaryTimer,
    aevOnSecondaryTimer,
    aevOnTertiaryTimer,
    aevOnPathSuccess,
    aevOnPathFailed,
    aevOnDialog,
    aevOnAction,
    aevOnAttacked,
    aevOnSelfKilled,
    aevOnTargetKilled,
    aevOnInFear,
    aevOnOutFear,
    aevOnBeginPrepare,
    aevOnBreakPerpare,
    aevOnCastSkill,
    aevOnCastSkillFailed,
    aevOnOutOfBattleZone,

    aevTotal,
};

// 用户自定义AI event从 KAI_USER_EVENT 开始
#define KAI_USER_EVENT 1000

class KAIVM
{
public:
    KAIVM();

    BOOL Setup(KCharacter* pCharacter, int nAIType);

    int  GetAIType();

    void Active();

    void FireEvent(int nEvent, DWORD dwEventSrc, int nEventParam);

    BOOL SetState(int nState);
    void SetPrimaryTimer(int nFrame);
    void SetSecondaryTimer(int nFrame);
    void SetTertiaryTimer(int nFrame);

    void AddupTotalDamageValue(int nDeltaDamageValue);

    void DebugAICurrentStateInfo();

public:
    int                 m_nDebugCount;
    uint64_t            m_ullRunTimeStatistic;

private:
    KCharacter*         m_pOwner;
    int                 m_nAIType;
    KAILogic*           m_pAILogic;
    KAIState*           m_pState;
    int                 m_nStateID;
    int                 m_nActionID;
    int                 m_nPrimaryTimerFrame;
    int                 m_nSecondaryTimerFrame;
    int                 m_nTertiaryTimerFrame;
    int                 m_nCurrentEvent; // 标示"正在某个Event的处理流程中"
    int                 m_nPendingEvent;
    DWORD               m_dwPendingEventSrc;
    int                 m_nPendingEventParam;
};

#endif
