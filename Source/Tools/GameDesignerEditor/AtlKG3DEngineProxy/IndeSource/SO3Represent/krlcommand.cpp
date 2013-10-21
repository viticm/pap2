#include "stdafx.h"
#include "./krlcommand.h"
#include "./SO3Represent.h"
#include "./case/actionobject/krlscene.h"
#include "./case/actionobject/krlcamera.h"
#include "./case/actionobject/krlremotecharacter.h"
#include "./case/actionobject/krldoodad.h"

static int UpdateLocalCharacterVisible()
{
    int nRetCode = false;
    int nResult = false;
    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        KRLScene* pRLScene = *itScene;
        KRLCamera* pRLCamera = NULL;

        pRLCamera = pRLScene->m_pRLCamera;
        KGLOG_PROCESS_ERROR(pRLCamera);

        nRetCode = pRLCamera->m_CharacterCamera.m_RLLocalCharacter.UpdateDisplayData(FALSE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    return nResult;
}

static int UpdateDoodadVisible()
{
    int nRetCode = false;
    int nResult = false;
    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        KRLScene* pRLScene = *itScene;

        KRLDoodadMgr::KPtrMap& Doodads = pRLScene->m_DoodadMgr.m_aDoodad;
        for (KRLDoodadMgr::KPtrMap::iterator it = Doodads.begin(); it != Doodads.end(); ++it)
        {
            KRLDoodad* pRLDoodad = it->second;

            nRetCode = pRLDoodad->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLDoodad->UpdateState(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        KRLDoodadMgr::KPtrMap& GCDoodads = pRLScene->m_DoodadGCMgr.m_aDoodad;
        for (KRLDoodadMgr::KPtrMap::iterator it = GCDoodads.begin(); it != GCDoodads.end(); ++it)
        {
            KRLDoodad* pRLDoodad = it->second;

            nRetCode = pRLDoodad->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLDoodad->UpdateState(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

static int UpdateRemoteCharacterVisible()
{
    int nRetCode = false;
    int nResult = false;
    KRLScenePtrArray& RLScenePtrArray = g_pRL->m_SceneMgr.m_apScene;

    for (KRLScenePtrArray::iterator itScene = RLScenePtrArray.begin(); itScene != RLScenePtrArray.end(); ++itScene)
    {
        KRLScene* pRLScene = *itScene;

        KRLRemoteCharacterMgr::KPtrMap& Characters = pRLScene->m_CharacterMgr.m_aCharacter;
        for (KRLRemoteCharacterMgr::KPtrMap::iterator it = Characters.begin(); it != Characters.end(); ++it)
        {
            KRLRemoteCharacter* pRLRemoteCharacter = it->second;

            nRetCode = pRLRemoteCharacter->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        KRLRemoteCharacterMgr::KPtrMap& GCCharacters = pRLScene->m_CharacterGCMgr.m_aCharacter;
        for (KRLRemoteCharacterMgr::KPtrMap::iterator it = GCCharacters.begin(); it != GCCharacters.end(); ++it)
        {
            KRLRemoteCharacter* pRLRemoteCharacter = it->second;

            nRetCode = pRLRemoteCharacter->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        KRLDoodadMgr::KPtrMap& Doodads = pRLScene->m_DoodadMgr.m_aDoodad;
        for (KRLDoodadMgr::KPtrMap::iterator it = Doodads.begin(); it != Doodads.end(); ++it)
        {
            KRLDoodad* pRLDoodad = it->second;

            nRetCode = pRLDoodad->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLDoodad->UpdateState(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        KRLDoodadMgr::KPtrMap& GCDoodads = pRLScene->m_DoodadGCMgr.m_aDoodad;
        for (KRLDoodadMgr::KPtrMap::iterator it = GCDoodads.begin(); it != GCDoodads.end(); ++it)
        {
            KRLDoodad* pRLDoodad = it->second;

            nRetCode = pRLDoodad->UpdateDisplayData(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = pRLDoodad->UpdateState(FALSE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

static int UpdateAllCharacterVisible()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = UpdateLocalCharacterVisible();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = UpdateRemoteCharacterVisible();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

static int ExecuteTraceCommand(LPCTSTR szCommand)
{
    int nRetCode = false;
    int nResult = false;

#ifdef RL_TRACE_OPTION
    if (_tcsicmp(szCommand, _T("enable trace")) == 0)
    {
        memcpy(&g_pRL->m_CurrentTraceOption, &g_pRL->m_TraceOption, sizeof(g_pRL->m_CurrentTraceOption));
        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("disable trace")) == 0)
    {
        memset(&g_pRL->m_CurrentTraceOption, 0, sizeof(g_pRL->m_CurrentTraceOption));
        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("trace all animation")) == 0)
    {
        g_pRL->m_TraceOption.bTrace3DAnimation = TRUE;
        g_pRL->m_TraceOption.bTracePlayerAnimation = TRUE;
        g_pRL->m_TraceOption.bTraceRidesAnimation = TRUE;
        g_pRL->m_TraceOption.bTraceNpcAnimation = TRUE;

        g_pRL->m_CurrentTraceOption.bTrace3DAnimation = TRUE;
        g_pRL->m_CurrentTraceOption.bTracePlayerAnimation = TRUE;
        g_pRL->m_CurrentTraceOption.bTraceRidesAnimation = TRUE;
        g_pRL->m_CurrentTraceOption.bTraceNpcAnimation = TRUE;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("trace 3d animation")) == 0)
    {
        g_pRL->m_TraceOption.bTrace3DAnimation = !g_pRL->m_TraceOption.bTrace3DAnimation;
        g_pRL->m_CurrentTraceOption.bTrace3DAnimation = g_pRL->m_TraceOption.bTrace3DAnimation;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("trace player animation")) == 0)
    {
        g_pRL->m_TraceOption.bTracePlayerAnimation = !g_pRL->m_TraceOption.bTracePlayerAnimation;
        g_pRL->m_CurrentTraceOption.bTracePlayerAnimation = g_pRL->m_TraceOption.bTracePlayerAnimation;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("trace rides animation")) == 0)
    {
        g_pRL->m_TraceOption.bTraceRidesAnimation = !g_pRL->m_TraceOption.bTraceRidesAnimation;
        g_pRL->m_CurrentTraceOption.bTraceRidesAnimation = g_pRL->m_TraceOption.bTraceRidesAnimation;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("trace npc animation")) == 0)
    {
        g_pRL->m_TraceOption.bTraceNpcAnimation = !g_pRL->m_TraceOption.bTraceNpcAnimation;
        g_pRL->m_CurrentTraceOption.bTraceNpcAnimation = g_pRL->m_TraceOption.bTraceNpcAnimation;

        goto Exit1;
    }
#endif

    goto Exit0;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}


struct KRLOptimizationCommand
{
    LPCTSTR szCommand;
    PBOOL pbOption;
    BOOL bResult;
    int (*pfnUpdate)();
};

static int ExecuteOptimizationCommand(LPCTSTR szCommand)
{
    int nRetCode = false;
    int nResult = false;

    if (_tcsicmp(szCommand, _T("auto optimization")) == 0)
    {
        g_pRL->m_CurrentOptimizationOption.bAuto = TRUE;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("manual optimization")) == 0)
    {
        g_pRL->m_CurrentOptimizationOption.bAuto = FALSE;

        memset(&g_pRL->m_CurrentOptimizationOption, 0, sizeof(g_pRL->m_CurrentOptimizationOption));

        nRetCode = UpdateAllCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateDoodadVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("enable optimization")) == 0)
    {
        memcpy(&g_pRL->m_CurrentOptimizationOption, &g_pRL->m_OptimizationOption, sizeof(g_pRL->m_CurrentOptimizationOption));

        nRetCode = UpdateAllCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateDoodadVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("disable optimization")) == 0)
    {
        memset(&g_pRL->m_CurrentOptimizationOption, 0, sizeof(g_pRL->m_CurrentOptimizationOption));

        nRetCode = UpdateAllCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateDoodadVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsncmp(szCommand, _T("optimization level"), _tcslen(_T("optimization level"))) == 0)
    {
        DWORD dwLevel = 0;

        nRetCode = _stscanf_s(szCommand, _T("optimization level %u"), &dwLevel) == 1;
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = Optimize(dwLevel);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateAllCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = UpdateDoodadVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("hide player")) == 0)
    {
        g_pRL->m_OptimizationOption.bHideEnemyPlayer = TRUE;
        g_pRL->m_OptimizationOption.bHideNeutralityPlayer = TRUE;
        g_pRL->m_OptimizationOption.bHidePartyPlayer = TRUE;
        g_pRL->m_OptimizationOption.bHideAllyPlayer = TRUE;

        g_pRL->m_CurrentOptimizationOption.bHideEnemyPlayer = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHideNeutralityPlayer = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHidePartyPlayer = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHideAllyPlayer = TRUE;

        nRetCode = UpdateRemoteCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("show player")) == 0)
    {
        g_pRL->m_OptimizationOption.bHideEnemyPlayer = FALSE;
        g_pRL->m_OptimizationOption.bHideNeutralityPlayer = FALSE;
        g_pRL->m_OptimizationOption.bHidePartyPlayer = FALSE;
        g_pRL->m_OptimizationOption.bHideAllyPlayer = FALSE;

        g_pRL->m_CurrentOptimizationOption.bHideEnemyPlayer = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHideNeutralityPlayer = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHidePartyPlayer = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHideAllyPlayer = FALSE;

        nRetCode = UpdateRemoteCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("hide npc")) == 0)
    {
        g_pRL->m_OptimizationOption.bHideEnemyNpc = TRUE;
        g_pRL->m_OptimizationOption.bHideNeutralityNpc = TRUE;
        g_pRL->m_OptimizationOption.bHidePartyNpc = TRUE;
        g_pRL->m_OptimizationOption.bHideAllyNpc = TRUE;

        g_pRL->m_CurrentOptimizationOption.bHideEnemyNpc = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHideNeutralityNpc = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHidePartyNpc = TRUE;
        g_pRL->m_CurrentOptimizationOption.bHideAllyNpc = TRUE;

        nRetCode = UpdateRemoteCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("show npc")) == 0)
    {
        g_pRL->m_OptimizationOption.bHideEnemyNpc = FALSE;
        g_pRL->m_OptimizationOption.bHideNeutralityNpc = FALSE;
        g_pRL->m_OptimizationOption.bHidePartyNpc = FALSE;
        g_pRL->m_OptimizationOption.bHideAllyNpc = FALSE;

        g_pRL->m_CurrentOptimizationOption.bHideEnemyNpc = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHideNeutralityNpc = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHidePartyNpc = FALSE;
        g_pRL->m_CurrentOptimizationOption.bHideAllyNpc = FALSE;

        nRetCode = UpdateRemoteCharacterVisible();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }

    static KRLOptimizationCommand s_Commands[] =
    {
        { _T("hide self"), &g_pRL->m_CurrentOptimizationOption.bHideSelf, TRUE, UpdateLocalCharacterVisible },
        { _T("show self"), &g_pRL->m_CurrentOptimizationOption.bHideSelf, FALSE, UpdateLocalCharacterVisible },
        { _T("hide enemy player"), &g_pRL->m_CurrentOptimizationOption.bHideEnemyPlayer, TRUE, UpdateRemoteCharacterVisible },
        { _T("show enemy player"), &g_pRL->m_CurrentOptimizationOption.bHideEnemyPlayer, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide neutrality player"), &g_pRL->m_CurrentOptimizationOption.bHideNeutralityPlayer, TRUE, UpdateRemoteCharacterVisible },
        { _T("show neutrality player"), &g_pRL->m_CurrentOptimizationOption.bHideNeutralityPlayer, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide party player"), &g_pRL->m_CurrentOptimizationOption.bHidePartyPlayer, TRUE, UpdateRemoteCharacterVisible },
        { _T("show party player"), &g_pRL->m_CurrentOptimizationOption.bHidePartyPlayer, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide ally player"), &g_pRL->m_CurrentOptimizationOption.bHideAllyPlayer, TRUE, UpdateRemoteCharacterVisible },
        { _T("show ally player"), &g_pRL->m_CurrentOptimizationOption.bHideAllyPlayer, FALSE, UpdateRemoteCharacterVisible },

        { _T("hide enemy npc"), &g_pRL->m_CurrentOptimizationOption.bHideEnemyNpc, TRUE, UpdateRemoteCharacterVisible },
        { _T("show enemy npc"), &g_pRL->m_CurrentOptimizationOption.bHideEnemyNpc, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide neutrality npc"), &g_pRL->m_CurrentOptimizationOption.bHideNeutralityNpc, TRUE, UpdateRemoteCharacterVisible },
        { _T("show neutrality npc"), &g_pRL->m_CurrentOptimizationOption.bHideNeutralityNpc, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide party npc"), &g_pRL->m_CurrentOptimizationOption.bHidePartyNpc, TRUE, UpdateRemoteCharacterVisible },
        { _T("show party npc"), &g_pRL->m_CurrentOptimizationOption.bHidePartyNpc, FALSE, UpdateRemoteCharacterVisible },
        { _T("hide ally npc"), &g_pRL->m_CurrentOptimizationOption.bHideAllyNpc, TRUE, UpdateRemoteCharacterVisible },
        { _T("show ally npc"), &g_pRL->m_CurrentOptimizationOption.bHideAllyNpc, FALSE, UpdateRemoteCharacterVisible },

        { _T("enable npc uniform"), &g_pRL->m_CurrentOptimizationOption.bNpcUniform, TRUE, UpdateRemoteCharacterVisible },
        { _T("disable npc uniform"), &g_pRL->m_CurrentOptimizationOption.bNpcUniform, FALSE, UpdateRemoteCharacterVisible },

        { _T("enable self uniform"), &g_pRL->m_CurrentOptimizationOption.bSelfUniform, TRUE, UpdateLocalCharacterVisible },
        { _T("disable self uniform"), &g_pRL->m_CurrentOptimizationOption.bSelfUniform, FALSE, UpdateLocalCharacterVisible },
        { _T("enable player unique uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerUniqueUniform, TRUE, UpdateAllCharacterVisible },
        { _T("disable player unique uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerUniqueUniform, FALSE, UpdateAllCharacterVisible },
        { _T("enable player gender uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerGenderUniform, TRUE, UpdateAllCharacterVisible },
        { _T("disable player gender uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerGenderUniform, FALSE, UpdateAllCharacterVisible },
        { _T("enable player roletype uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerRoleTypeUniform, TRUE, UpdateAllCharacterVisible },
        { _T("disable player roletype uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerRoleTypeUniform, FALSE, UpdateAllCharacterVisible },
        { _T("enable player force uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerForceUniform, TRUE, UpdateAllCharacterVisible },
        { _T("disable player force uniform"), &g_pRL->m_CurrentOptimizationOption.bPlayerForceUniform, FALSE, UpdateAllCharacterVisible },

        { _T("enable skill animation uniform"), &g_pRL->m_CurrentOptimizationOption.bSkillAnimationUniform, TRUE, NULL },
        { _T("disable skill animation uniform"), &g_pRL->m_CurrentOptimizationOption.bSkillAnimationUniform, FALSE, NULL },

        { _T("enable animation blend"), &g_pRL->m_CurrentOptimizationOption.bDisableAnimationBlend, FALSE, NULL },
        { _T("disable animation blend"), &g_pRL->m_CurrentOptimizationOption.bDisableAnimationBlend, TRUE, NULL },
    
        { _T("hide doodad"), &g_pRL->m_CurrentOptimizationOption.bHideDoodad, TRUE, UpdateDoodadVisible },
        { _T("show doodad"), &g_pRL->m_CurrentOptimizationOption.bHideDoodad, FALSE, UpdateDoodadVisible },
        { _T("hide corpse"), &g_pRL->m_CurrentOptimizationOption.bHideCorpse, TRUE, UpdateDoodadVisible },
        { _T("show corpse"), &g_pRL->m_CurrentOptimizationOption.bHideCorpse, FALSE, UpdateDoodadVisible },
        { _T("hide doodad effect"), &g_pRL->m_CurrentOptimizationOption.bHideDoodadEffect, TRUE, UpdateDoodadVisible },
        { _T("show doodad effect"), &g_pRL->m_CurrentOptimizationOption.bHideDoodadEffect, FALSE, UpdateDoodadVisible },
        { _T("hide corpse effect"), &g_pRL->m_CurrentOptimizationOption.bHideCorpseEffect, TRUE, UpdateDoodadVisible },
        { _T("show corpse effect"), &g_pRL->m_CurrentOptimizationOption.bHideCorpseEffect, FALSE, UpdateDoodadVisible },

        { _T("enable doodad uniform"), &g_pRL->m_CurrentOptimizationOption.bDoodadUniform, TRUE, UpdateDoodadVisible },
        { _T("disable doodad uniform"), &g_pRL->m_CurrentOptimizationOption.bDoodadUniform, FALSE, UpdateDoodadVisible },
        { _T("enable corpse uniform"), &g_pRL->m_CurrentOptimizationOption.bCorpseUniqueUniform, TRUE, UpdateDoodadVisible },
        { _T("disable corpse uniform"), &g_pRL->m_CurrentOptimizationOption.bCorpseUniqueUniform, FALSE, UpdateDoodadVisible },
    };

    for (size_t nCommand = 0; nCommand < _countof(s_Commands); ++nCommand)
    {
        KRLOptimizationCommand& Command = s_Commands[nCommand];

        if (_tcsicmp(Command.szCommand, szCommand) == 0)
        {
            *Command.pbOption = Command.bResult;

            if (Command.pfnUpdate)
            {
                nRetCode = Command.pfnUpdate();
                KGLOG_PROCESS_ERROR(nRetCode);
            }

            goto Exit1;
        }
    }

    goto Exit0;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static int ExecuteTerrainLeanCommand(LPCTSTR szCommand)
{
    int nRetCode = false;
    int nResult = false;

    if (_tcsicmp(szCommand, _T("enable terrain lean")) == 0)
    {
        g_pRL->m_TerrainLeanOption.bDisable = FALSE;
        g_pRL->m_CurrentTerrainLeanOption.bDisable = g_pRL->m_TerrainLeanOption.bDisable;

        goto Exit1;
    }
    if (_tcsicmp(szCommand, _T("disable terrain lean")) == 0)
    {
        g_pRL->m_TerrainLeanOption.bDisable = TRUE;
        g_pRL->m_CurrentTerrainLeanOption.bDisable = g_pRL->m_TerrainLeanOption.bDisable;

        goto Exit1;
    }

    goto Exit0;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int ExecuteRLCommand(LPCTSTR szCommand)
{
    int nRetCode = false;
    int nResult = false;

    ASSERT(szCommand);

    nRetCode = ExecuteTraceCommand(szCommand);
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = ExecuteOptimizationCommand(szCommand);
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = ExecuteTerrainLeanCommand(szCommand);
    KG_PROCESS_SUCCESS(nRetCode);

    // table
    if (_tcsicmp(szCommand, _T("reload table")) == 0)
    {
        nRetCode = g_pRL->m_TableList.Reset();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

