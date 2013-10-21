#ifndef KRLOPTION_H
#define KRLOPTION_H

#include "./case/render/krlresource.h"

#define RL_TRACE_OPTION 1

struct KRLOption
{
    BOOL bHeadSFXEnabled[CHARACTER_TYPE_END][CHARACTER_SFX_BBOX_END - CHARACTER_SFX_BBOX_BEGIN]; // TODO:
};

#ifdef RL_TRACE_OPTION

struct KRLTraceOption
{
    BOOL bTrace3DAnimation;
    BOOL bTracePlayerAnimation;
    BOOL bTraceRidesAnimation;
    BOOL bTraceNpcAnimation;
};

#endif // RL_TRACE_OPTION

struct KRLOptimizationOption
{
    BOOL bAuto;

    BOOL bHideDoodad;
    BOOL bHideCorpse;

    BOOL bHideDoodadEffect;
    BOOL bHideCorpseEffect;

    BOOL bHideEnemyPlayer;
    BOOL bHideNeutralityPlayer;
    BOOL bHidePartyPlayer;
    BOOL bHideAllyPlayer;

    BOOL bHideEnemyNpc;
    BOOL bHideNeutralityNpc;
    BOOL bHidePartyNpc;
    BOOL bHideAllyNpc;

    BOOL bHideSelf;
    BOOL bSelfUniform;

    BOOL bDoodadUniform;
    BOOL bCorpseUniqueUniform;

    BOOL bNpcUniform;

    BOOL bPlayerUniqueUniform;
    BOOL bPlayerGenderUniform;
    BOOL bPlayerRoleTypeUniform;
    BOOL bPlayerForceUniform;

    BOOL bSkillAnimationUniform;

    BOOL bDisableAnimationBlend;
};

struct KRLTerrainLeanOption
{
    BOOL bDisable;
};

#endif // KRLOPTION_H

